#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "functions.h"
#include "httpdata.h"
#include "tvgids_nl.h"


Channels TvGidsNL::GetChannels() const
{
	HttpData httpData;
	std::string channelsString = httpData.GetUrlContents("http://www.tvgids.nl/json/lists/channels.php");

	Channels channels = LoadFromJSON(channelsString);
	std::sort(channels.begin(), channels.end());

	return channels;
}

Programs TvGidsNL::GetPrograms(Channels channels, const ScanConfig& scanConfig) const
{
	int days = (scanConfig.days > 4)? 4: scanConfig.days;

	Programs programs;
	if (scanConfig.cache)
	{
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

		for (int day = 1; day <= days; ++day)
		{
			HttpData httpData;
			std::stringstream ss;
			ss << "http://www.tvgids.nl/json/lists/programs.php?channels=" << it->GetId() << "&day=" << day;
			std::string programsString = httpData.GetUrlContents(ss.str());

			LoadFromJSON(programs, *it, programsString);
		}
	}

	if (!scanConfig.quiet)
		OutputProgress(item, total, percent);

	if (scanConfig.cache)
	{
		int today = boost::lexical_cast<int>(boost::gregorian::to_iso_string(boost::gregorian::day_clock::local_day()));
		std::vector<Program> deleteList;

		for (Programs::iterator it = programs.begin(); it != programs.end(); ++it)
			if (today > boost::lexical_cast<int>(it->GetDateEnd().substr(0, 8)) ||
			    std::find(channels.begin(), channels.end(), it->GetChannel().GetId()) == channels.end())
				deleteList.push_back(*it);

		int total = deleteList.size();
		if (total > 0)
		{
			int percent = 0;
			int item = 0;

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
	}

	if (!scanConfig.fast)
	{
		int total = 0;
		int percent = 0;
		int item = 0;

		if (!scanConfig.quiet)
		{
			total = programs.size();
			std::cerr << "Fetching detailed info for " << total << " programmes:" << std::endl;
		}

		for (Programs::iterator it = programs.begin(); it != programs.end(); ++it, ++item)
		{
			if (!it->GetDetailsLoaded())
			{
				HttpData httpData;
				std::stringstream ss;
				ss << "http://www.tvgids.nl/json/lists/program.php?id=" << it->GetId();
				std::string programString = httpData.GetUrlContents(ss.str());

				LoadDetailsFromJSON(*it, programString);
			}

			if (!scanConfig.quiet)
				OutputProgress(item, total, percent);
		}
	}

	if (!scanConfig.quiet)
		OutputProgress(item, total, percent);

	std::sort(programs.begin(), programs.end());

	if (scanConfig.cache)
	{
		std::ofstream cacheOutFile(scanConfig.cacheFilename.c_str());
		if (cacheOutFile.good())
		{
			boost::archive::text_oarchive cacheOutArchive(cacheOutFile);
			cacheOutArchive << programs;
		}
	}

	return programs;
}

Channels TvGidsNL::LoadFromJSON(std::string json) const
{
	std::stringstream jsonStream;
	jsonStream << "{ \"root\": " << json << " }";

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	Channels channels;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("root"))
	{
		Channel channel;
		channel.SetId(val.second.get<int>("id"));
		channel.SetName(val.second.get<std::string>("name"));
		channel.SetNameShort(val.second.get<std::string>("name_short"));

		channels.push_back(channel);
	}

	return channels;
}

void TvGidsNL::LoadFromJSON(Programs& programs, Channel channel, std::string json) const
{
	std::stringstream jsonStream;
	jsonStream << json;

	std::stringstream channelIdStream;
	channelIdStream << channel.GetId();

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child(channelIdStream.str().c_str()))
	{
		Program program(channel);

		program.SetId(val.second.get<int>("db_id"));

		if (std::find(programs.begin(), programs.end(), program.GetId()) != programs.end())
			continue;

		program.SetTitle(val.second.get<std::string>("titel"));
		program.SetType(val.second.get<std::string>("soort"));
		program.SetGenre(val.second.get<std::string>("genre"));
		program.SetRating(val.second.get<std::string>("kijkwijzer"));
		program.SetDateStart(val.second.get<std::string>("datum_start"));
		program.SetDateEnd(val.second.get<std::string>("datum_end"));

		boost::optional<int> articleId = val.second.get_optional<int>("artikel_id");
		if (articleId.is_initialized())
			program.SetArticleId(articleId.get());

		boost::optional<std::string> articleTitle = val.second.get_optional<std::string>("artikel_titel");
		if (articleTitle.is_initialized())
			program.SetArticleTitle(articleTitle.get());

		programs.push_back(program);
	}
}

void TvGidsNL::LoadDetailsFromJSON(Program& program, std::string json) const
{
	std::stringstream jsonStream;
	jsonStream << "{ \"root\": [" << json << "] }";

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("root"))
	{
		boost::optional<std::string> synopsis = val.second.get_optional<std::string>("synop");
		if (synopsis.is_initialized())
			program.SetSynopsis(synopsis.get());

		boost::optional<std::string> hosts = val.second.get_optional<std::string>("presentatie");
		if (hosts.is_initialized())
			program.SetHosts(hosts.get());

		boost::optional<std::string> actors = val.second.get_optional<std::string>("acteursnamen_rolverdeling");
		if (actors.is_initialized())
			program.SetActors(actors.get());

		boost::optional<std::string> director = val.second.get_optional<std::string>("regisseur");
		if (director.is_initialized())
			program.SetDirector(director.get());
	}

	program.SetDetailsLoaded(true);
}

