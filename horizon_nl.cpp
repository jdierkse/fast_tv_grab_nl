#include <ctime>
#include <stdint.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>
#include "functions.h"
#include "horizon_nl.h"


HorizonNL::HorizonNL() :
	m_threadPool(4)
{
}

Channels HorizonNL::GetChannels() const
{
	HttpData httpData;
	std::string channelsString = httpData.GetUrlContents("https://web-api-pepper.horizon.tv/oesp/api/NL/nld/web/channels");

	Channels channels = LoadFromJSON(channelsString);
	std::sort(channels.begin(), channels.end());

	return channels;
}

Programs HorizonNL::GetPrograms(Channels& channels, const ScanConfig& scanConfig)
{
	Programs programs;

	int total = 0;
	int percent = 0;
	int item = 0;

	if (!scanConfig.quiet)
	{
		total = channels.size();
		std::cerr << "Fetching detailed info for " << total << " channels:" << std::endl;
	}

	boost::shared_ptr<HttpData> pHttpData = boost::make_shared<HttpData>();

	for (Channels::iterator it = channels.begin(); it != channels.end(); ++it, ++item)
		m_threadPool.Execute(boost::bind(&HorizonNL::GetChannelPrograms, this, *it, pHttpData, scanConfig, item, total));

	m_threadPool.Join();

	if (!scanConfig.quiet)
	{
		OutputProgress(item, total, percent);
		std::cerr << "Converting data:" << std::endl;
	}

	{
		total = m_programs.size();
		percent = 0;
		item = 0;

		boost::unique_lock<boost::mutex> lock(m_mutex);
		for (std::vector<std::pair<Channel, std::string> >::iterator it = m_programs.begin(); it != m_programs.end(); ++it, ++item)
		{
			LoadFromJSON(programs, it->first, it->second);
			if (!scanConfig.quiet)
				OutputProgress(item, total, percent);
		}
	}

	if (!scanConfig.quiet)
		OutputProgress(item, total, percent);

	std::sort(programs.begin(), programs.end());

	return programs;
}

void HorizonNL::GetChannelPrograms(Channel& channel, const boost::shared_ptr<HttpData>& pHttpData, const ScanConfig& scanConfig, int item, int total)
{
	int days = scanConfig.days;
	int now = time(0);
	int today = now - (now % 86400);
	int start = today;
	int end = today + days * 24 * 60 * 60;

	if (!scanConfig.quiet)
	{
		boost::unique_lock<boost::mutex> lock(m_mutex);
		OutputProgress(item, total, 0);
	}

	std::stringstream ss;
	ss << "https://web-api-pepper.horizon.tv/oesp/api/NL/nld/web/listings?byStationId=" << channel.InternalId() << "&byStartTime=" << start << "000~&byEndTime=~" << end << "000&sort=startTime";
	std::string programString = pHttpData->GetUrlContents(ss.str());

	boost::unique_lock<boost::mutex> lock(m_mutex);
	m_programs.push_back(std::make_pair(channel, programString));
}

Channels HorizonNL::LoadFromJSON(const std::string& json) const
{
	std::stringstream jsonStream;
	jsonStream << json;

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	Channels channels;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("channels"))
	{
		Channel channel;
		channel.Id(val.second.get<int>("channelNumber"));

		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, val.second.get_child("stationSchedules"))
		{
			boost::property_tree::ptree c = v.second.get_child("station");
			channel.InternalId(c.get<std::string>("id"));
			channel.Name(c.get<std::string>("title"));

			BOOST_FOREACH(boost::property_tree::ptree::value_type &i, c.get_child("images"))
			{
				std::string assetType = i.second.get<std::string>("assetType");
				if (assetType == "station-logo-large")
					channel.IconUrl(i.second.get<std::string>("url"));
			}
		}

		channels.push_back(channel);
	}

	return channels;
}

void HorizonNL::LoadFromJSON(Programs& programs, const Channel& channel, const std::string& json) const
{
	std::stringstream jsonStream;
	jsonStream << json;

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("listings"))
	{
		Program program(channel);
		boost::property_tree::ptree p;
		try
		{
			p = val.second.get_child("program");
		}
		catch (...)
		{
			continue;
		}

		program.InternalId(p.get<std::string>("id"));

		if (std::find(programs.begin(), programs.end(), program.InternalId()) != programs.end())
			continue;

		program.Title(p.get<std::string>("title"));

		boost::optional<std::string> longDescription = p.get_optional<std::string>("longDescription");
		if (longDescription.is_initialized())
		{
			program.Synopsis(longDescription.get());
		}
		else
		{
			boost::optional<std::string> description = p.get_optional<std::string>("description");
			if (description.is_initialized())
			{
				program.Synopsis(description.get());
			}
			else
			{
				boost::optional<std::string> shortDescription = p.get_optional<std::string>("shortDescription");
				if (shortDescription.is_initialized())
					program.Synopsis(shortDescription.get());
			}
		}

		program.Country(p.get<std::string>("countryCode"));

		boost::optional<std::string> medium = p.get_optional<std::string>("medium");
		if (medium.is_initialized())
			program.Type(medium.get());

		program.DateStart(ConvertDate(val.second.get<int64_t>("startTime") / 1000));
		program.DateEnd(ConvertDate(val.second.get<int64_t>("endTime") / 1000));

		boost::optional<std::string> parentalRating = p.get_optional<std::string>("parentalRating");
		if (parentalRating.is_initialized())
			program.Rating(parentalRating.get());

		BOOST_FOREACH(boost::property_tree::ptree::value_type &g, p.get_child("categories"))
		{
			// Just pick the first category...
			program.Genre(ConvertGenre(g.second.get<std::string>("title")));
			break;
		}

		bool castEmpty = true;
		std::stringstream cast;
		BOOST_FOREACH(boost::property_tree::ptree::value_type &a, p.get_child("cast"))
		{
			if (!castEmpty)
				cast << ", ";
			cast << a.second.data();
			castEmpty = false;
		}

		if (!castEmpty)
			program.Actors(cast.str());

		bool directorEmpty = true;
		std::stringstream director;
		BOOST_FOREACH(boost::property_tree::ptree::value_type &d, p.get_child("directors"))
		{
			if (!directorEmpty)
				director << ", ";
			director << d.second.data();
			directorEmpty = false;
		}

		if (!directorEmpty)
			program.Director(director.str());

		programs.push_back(program);
	}
}

std::string HorizonNL::ConvertGenre(const std::string& genre) const
{
	if (genre == "news" || genre == "nieuws" ||
	    genre == "nieuws/nieuws" || genre == "nieuws/nieuws/discussie" ||
	    genre == "nieuws/nieuws/documentaire" || genre == "NIEUWS")
		return "News / Current affairs";
	if (genre == "show|game show" || genre == "entertainment" ||
	    genre == "show|spelshow" || genre == "show|spelshow/show" ||
	    genre == "show|spelshow/show/talkshow" || genre == "show/spelshow/show" ||
	    genre == "show" || genre == "SHOW")
		return "Show / Game show";
	if (genre == "kids|youth" || genre == "kids|jeugd" ||
	    genre == "kids|jeugd/kids|jeugd" || genre == "kids/jeugd/kids/jeugd" ||
	    genre == "kids/jeugd" || genre == "KIDS/JEUGD")
		return "Children's / Youth programmes";
	if (genre == "movie" || genre == "TV drama" ||
	    genre == "action" || genre == "comedy" ||
	    genre == "drama" || genre == "horror" ||
	    genre == "romance" || genre == "sci-fi" ||
	    genre == "thriller" || genre == "historical" ||
	    genre == "adventure" || genre == "musical" ||
	    genre == "western" || genre == "romantiek" ||
	    genre == "komedie" || genre == "actie" ||
	    genre == "speelfilm" || genre == "avontuur" ||
	    genre == "melodrama" || genre == "historisch" ||
	    genre == "ACTIE" || genre == "AVONTUUR" ||
	    genre == "DRAMA" || genre == "HISTORISCH" ||
	    genre == "HORROR" || genre == "KOMEDIE" ||
	    genre == "ROMANTIEK" || genre == "SCI-FI" ||
	    genre == "SPEELFILM" || genre == "THRILLER" ||
	    genre == "TV DRAMA" || genre == "WESTERN")
		return "Movie / Drama";
	if (genre == "educational" || genre == "educatie" ||
	    genre == "educatie/educatie" || genre == "EDUCATIE")
		return "Education / Science / Factual topics";
	if (genre == "sports" || genre == "sport" ||
	    genre == "sport/sport" || genre == "SPORT")
		return "Sports";
	if (genre == "leisure" || genre == "adult" ||
	    genre == "special" || genre == "vrije tijd" ||
	    genre == "vrije tijd/vrije tijd" || genre == "vrije tijd/vrije tijd/gezondheid" ||
	    genre == "erotiek" || genre == "EROTIEK" ||
	    genre == "SPECIAL" || genre == "VRIJE TIJD")
		return "Leisure hobbies";
	if (genre == "music|dance" || genre == "muziek|dans" ||
	    genre == "muziek" || genre == "MUSICAL" ||
	    genre == "MUZIEK")
		return "Music / Ballet / Dance";
	if (genre == "social|political" || genre == "maatschappij" ||
	    genre == "maatschappij/maatschappelijk" || genre == "maatschappelijk" ||
	    genre == "MAATSCHAPPELIJK")
		return "Social / Political issues / Economics";
	if (genre == "arts|culture" || genre == "kunst|cultuur" ||
	    genre == "kunst/cultuur/kunst/cultuur" || genre == "kunst/cultuur" ||
	    genre == "KUNST/CULTUUR")
		return "Arts / Culture (without music)";

	return genre;
}

std::string HorizonNL::ConvertDate(int date) const
{
	std::stringstream ss;
	boost::posix_time::ptime time = boost::posix_time::from_time_t(date);
	ss << boost::regex_replace(boost::posix_time::to_iso_string(time), boost::regex("[A-Z]"), "") << " UTC";

	return ss.str();
}
