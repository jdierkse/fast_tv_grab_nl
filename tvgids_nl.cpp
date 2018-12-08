#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include "functions.h"
#include "httpdata.h"
#include "tvgids_nl.h"


Channels TvGidsNL::GetChannels() const
{
	HttpData httpData;
	std::string channelsString = httpData.GetUrlContents("http://json.tvgids.nl/v4/channels");

	Channels channels = LoadFromJSON(channelsString);
	std::sort(channels.begin(), channels.end());

	return channels;
}

Programs TvGidsNL::GetPrograms(Channels& channels, const ScanConfig& scanConfig)
{
	int days = (scanConfig.days > 4)? 4: scanConfig.days;

	Programs programs;

	std::ifstream cacheInFile(scanConfig.cacheFilename.c_str());
	if (cacheInFile.good())
	{
		try
		{
			boost::archive::text_iarchive cacheInArchive(cacheInFile);
			cacheInArchive >> programs;
		}
		catch (const boost::archive::archive_exception&)
		{
			programs = Programs();
		}
	}

	int total = channels.size();
	int percent = 0;
	int item = 0;

	if (!scanConfig.quiet)
		std::cerr << "Fetching basic information for " << channels.size() << " channels:" << std::endl;

	for (Channels::iterator it = channels.begin(); it != channels.end(); ++it, ++item)
	{
		if (!scanConfig.quiet)
			OutputProgress(item, total, percent);

		for (int day = 0; day < days; ++day)
		{
			HttpData httpData;
			std::stringstream ss;
			ss << "http://json.tvgids.nl/v4/programs/?channels=" << it->Id() << "&day=" << day;
			std::string programsString = httpData.GetUrlContents(ss.str());

			LoadFromJSON(programs, *it, programsString);
		}
	}

	if (!scanConfig.quiet)
		OutputProgress(item, total, percent);

	int today = boost::lexical_cast<int>(boost::gregorian::to_iso_string(boost::gregorian::day_clock::local_day()));
	std::vector<Program> deleteList;

	for (Programs::iterator it = programs.begin(); it != programs.end(); ++it)
		if (today > boost::lexical_cast<int>(it->DateEnd().substr(0, 8)) ||
		    std::find(channels.begin(), channels.end(), it->Channel().Id()) == channels.end())
			deleteList.push_back(*it);

	total = deleteList.size();
	if (total > 0)
	{
		percent = 0;
		item = 0;

		if (!scanConfig.quiet)
			std::cerr << "Removing " << total << " old programmes from cache" << std::endl;

		for (std::vector<Program>::iterator it = deleteList.begin(); it != deleteList.end(); ++it, ++item)
		{
			programs.Remove(*it);

			if (!scanConfig.quiet)
				OutputProgress(item, total, percent);
		}

		if (!scanConfig.quiet)
			OutputProgress(item, total, percent);
	}

	std::sort(programs.begin(), programs.end());

	std::ofstream cacheOutFile(scanConfig.cacheFilename.c_str());
	if (cacheOutFile.good())
	{
		boost::archive::text_oarchive cacheOutArchive(cacheOutFile);
		cacheOutArchive << programs;
	}

	return programs;
}

Channels TvGidsNL::LoadFromJSON(const std::string& json) const
{
	std::stringstream jsonStream;
	jsonStream << json;

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	Channels channels;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("data"))
	{
		Channel channel;
		channel.Id(val.second.get<int>("ch_id"));
		channel.Name(val.second.get<std::string>("ch_name"));
		channel.NameShort(val.second.get<std::string>("ch_short"));

		channels.push_back(channel);
	}

	return channels;
}

void TvGidsNL::LoadFromJSON(Programs& programs, const Channel& channel, const std::string& json) const
{
	std::stringstream jsonStream;
	jsonStream << json;

	std::stringstream channelIdStream;
	channelIdStream << channel.Id();

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("data").get_child(channelIdStream.str().c_str()).get_child("prog"))
	{
		Program program(channel);

		program.Id(val.second.get<int>("db_id"));

		if (std::find(programs.begin(), programs.end(), program.Id()) != programs.end())
			continue;

		program.Title(val.second.get<std::string>("title"));
		program.DateStart(ConvertDate(val.second.get<int>("s")));
		program.DateEnd(ConvertDate(val.second.get<int>("e")));

		boost::optional<int> genre = val.second.get_optional<int>("g_id");
		if (genre.is_initialized())
			program.Genre(ConvertGenre(genre.get()));

		boost::optional<std::string> synopsis = val.second.get_optional<std::string>("descr");
		if (synopsis.is_initialized())
			program.Synopsis(synopsis.get());

		programs.push_back(program);
	}
}

std::string TvGidsNL::ConvertGenre(int genre) const
{
	if (genre == 13)
		return "News / Current affairs";
	if (genre ==  1 || genre ==  2 ||
	    genre ==  3)
		return "Show / Game show";
	if (genre == 8)
		return "Children's / Youth programmes";
	if (genre ==  6 || genre == 10 ||
			genre == 16)
		return "Movie / Drama";
	if (genre ==  4 || genre ==  7 ||
	    genre == 12 || genre == 19)
		return "Education / Science / Factual topics";
	if (genre == 17)
		return "Sports";
	if (genre == 11)
		return "Music / Ballet / Dance";
	if (genre == 15)
		return "Social / Political issues / Economics";
	if (genre ==  9 || genre == 18)
		return "Arts / Culture (without music)";
	//if (genre == 21 || genre == 23)

	return "Other";
}

std::string TvGidsNL::ConvertDate(int date) const
{
	std::stringstream ss;
	boost::posix_time::ptime time = boost::posix_time::from_time_t(date);
	ss << boost::regex_replace(boost::posix_time::to_iso_string(time), boost::regex("[A-Z]"), "") << " UTC";

	return ss.str();
}
