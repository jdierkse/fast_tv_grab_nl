#include <ctime>
#include <stdint.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include "functions.h"
#include "httpdata.h"
#include "upc_nl.h"


Channels UPCNL::GetChannels() const
{
	HttpData httpData;
	std::string channelsString = httpData.GetUrlContents("https://www.horizon.tv/oesp/api/NL/nld/web/channels");

	Channels channels = LoadFromJSON(channelsString);
	std::sort(channels.begin(), channels.end());

	return channels;
}

Programs UPCNL::GetPrograms(Channels channels, const ScanConfig& scanConfig) const
{
	int days = scanConfig.days;

	Programs programs;

	int total = 0;
	int percent = 0;
	int item = 0;

	if (!scanConfig.quiet)
	{
		total = channels.size();
		std::cerr << "Fetching detailed info for " << total << " channels:" << std::endl;
	}

	for (Channels::iterator it = channels.begin(); it != channels.end(); ++it, ++item)
	{
		int now = time(0);
		int today = now - (now % 86400);
		int start = today;
		int end = today + days * 24 * 60 * 60;

		if (!scanConfig.quiet)
			OutputProgress(item, total, percent);

		HttpData httpData;
		std::stringstream ss;
		ss << "https://www.horizon.tv/oesp/api/NL/nld/web/listings?byStationId=" << it->GetInternalId() << "&byStartTime=" << start << "000~&byEndTime=~" << end << "000&sort=startTime";
		std::string programString = httpData.GetUrlContents(ss.str());

		LoadFromJSON(programs, *it, programString);
	}

	if (!scanConfig.quiet)
		OutputProgress(item, total, percent);

	std::sort(programs.begin(), programs.end());

	return programs;
}

Channels UPCNL::LoadFromJSON(std::string json) const
{
	std::stringstream jsonStream;
	jsonStream << json;

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	Channels channels;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("channels"))
	{
		Channel channel;
		channel.SetId(val.second.get<int>("channelNumber"));

		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, val.second.get_child("stationSchedules"))
		{
			boost::property_tree::ptree c = v.second.get_child("station");
			channel.SetInternalId(c.get<std::string>("id"));
			channel.SetName(c.get<std::string>("title"));

			BOOST_FOREACH(boost::property_tree::ptree::value_type &i, c.get_child("images"))
			{
				std::string assetType = i.second.get<std::string>("assetType");
				if (assetType == "station-logo-large")
					channel.SetIconUrl(i.second.get<std::string>("url"));
			}
		}

		channels.push_back(channel);
	}

	return channels;
}

void UPCNL::LoadFromJSON(Programs& programs, Channel channel, std::string json) const
{
	std::stringstream jsonStream;
	jsonStream << json;

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("listings"))
	{
		Program program(channel);
		boost::property_tree::ptree p = val.second.get_child("program");

		program.SetInternalId(p.get<std::string>("id"));

		if (std::find(programs.begin(), programs.end(), program.GetInternalId()) != programs.end())
			continue;

		program.SetTitle(p.get<std::string>("title"));

		boost::optional<std::string> longDescription = p.get_optional<std::string>("longDescription");
		if (longDescription.is_initialized())
		{
			program.SetSynopsis(longDescription.get());
		}
		else
		{
			boost::optional<std::string> description = p.get_optional<std::string>("description");
			if (description.is_initialized())
			{
				program.SetSynopsis(description.get());
			}
			else
			{
				boost::optional<std::string> shortDescription = p.get_optional<std::string>("shortDescription");
				if (shortDescription.is_initialized())
					program.SetSynopsis(shortDescription.get());
			}
		}

		program.SetCountry(p.get<std::string>("countryCode"));

		boost::optional<std::string> medium = p.get_optional<std::string>("medium");
		if (medium.is_initialized())
			program.SetType(medium.get());

		program.SetDateStart(ConvertDate(val.second.get<int64_t>("startTime") / 1000));
		program.SetDateEnd(ConvertDate(val.second.get<int64_t>("endTime") / 1000));

		boost::optional<std::string> parentalRating = p.get_optional<std::string>("parentalRating");
		if (parentalRating.is_initialized())
			program.SetRating(parentalRating.get());

		BOOST_FOREACH(boost::property_tree::ptree::value_type &g, p.get_child("categories"))
		{
			// Just pick the first category...
			program.SetGenre(ConvertGenre(g.second.get<std::string>("title")));
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
			program.SetActors(cast.str());

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
			program.SetDirector(director.str());

		programs.push_back(program);
	}
}

std::string UPCNL::ConvertGenre(std::string genre) const
{
	if (genre == "news")
		return "News / Current affairs";
	if (genre == "show|game show" || genre == "entertainment")
		return "Show / Game show";
	if (genre == "kids|youth")
		return "Children's / Youth programmes";
	if (genre == "movie" || genre == "TV drama" ||
	    genre == "action" || genre == "comedy" ||
	    genre == "drama" || genre == "horror" ||
	    genre == "romance" || genre == "sci-fi" ||
	    genre == "thriller")
		return "Movie / Drama";
	if (genre == "educational")
		return "Education / Science / Factual topics";
	if (genre == "sports")
		return "Sports";
	if (genre == "leisure" || genre == "adult" ||
	    genre == "special")
		return "Leisure hobbies";
	if (genre == "music|dance")
		return "Music / Ballet / Dance";
	if (genre == "social|political")
		return "Social / Political issues / Economics";
	if (genre == "arts|culture")
		return "Arts / Culture (without music)";

	return genre;
}

std::string UPCNL::ConvertDate(int date) const
{
	std::stringstream ss;
	boost::posix_time::ptime time = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(boost::posix_time::from_time_t(date));
	ss << boost::regex_replace(boost::posix_time::to_iso_string(time), boost::regex("[A-Z]"), "") << " +0100";

	return ss.str();
}

