#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
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
	std::string channelsString = httpData.GetUrlContents("http://www.tvgids.nl/json/lists/channels.php");

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

		for (int day = 1; day <= days; ++day)
		{
			HttpData httpData;
			std::stringstream ss;
			ss << "http://www.tvgids.nl/json/lists/programs.php?channels=" << it->Id() << "&day=" << day;
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

	total = 0;
	percent = 0;
	item = 0;

	if (!scanConfig.quiet)
	{
		total = programs.size();
		std::cerr << "Fetching detailed info for " << total << " programmes:" << std::endl;
	}

	for (Programs::iterator it = programs.begin(); it != programs.end(); ++it, ++item)
	{
		if (!it->DetailsLoaded())
		{
			HttpData httpData;
			std::stringstream ss;
			ss << "http://www.tvgids.nl/json/lists/program.php?id=" << it->Id();
			std::string programString = httpData.GetUrlContents(ss.str());

			LoadDetailsFromJSON(*it, programString);
		}

		if (!scanConfig.quiet)
			OutputProgress(item, total, percent);
	}

	if (!scanConfig.quiet)
		OutputProgress(item, total, percent);

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
	jsonStream << "{ \"root\": " << json << " }";

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	Channels channels;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("root"))
	{
		Channel channel;
		channel.Id(val.second.get<int>("id"));
		channel.Name(val.second.get<std::string>("name"));
		channel.NameShort(val.second.get<std::string>("name_short"));

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

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child(channelIdStream.str().c_str()))
	{
		Program program(channel);

		program.Id(val.second.get<int>("db_id"));

		if (std::find(programs.begin(), programs.end(), program.Id()) != programs.end())
			continue;

		program.Title(val.second.get<std::string>("titel"));
		program.Type(val.second.get<std::string>("soort"));
		program.Genre(ConvertGenre(val.second.get<std::string>("genre")));
		program.Rating(val.second.get<std::string>("kijkwijzer"));
		program.DateStart(ConvertDate(val.second.get<std::string>("datum_start")));
		program.DateEnd(ConvertDate(val.second.get<std::string>("datum_end")));

		boost::optional<int> articleId = val.second.get_optional<int>("artikel_id");
		if (articleId.is_initialized())
			program.ArticleId(articleId.get());

		boost::optional<std::string> articleTitle = val.second.get_optional<std::string>("artikel_titel");
		if (articleTitle.is_initialized())
			program.ArticleTitle(articleTitle.get());

		programs.push_back(program);
	}
}

void TvGidsNL::LoadDetailsFromJSON(Program& program, const std::string& json) const
{
	std::stringstream jsonStream;
	jsonStream << "{ \"root\": [" << json << "] }";

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("root"))
	{
		boost::optional<std::string> synopsis = val.second.get_optional<std::string>("synop");
		if (synopsis.is_initialized())
			program.Synopsis(synopsis.get());

		boost::optional<std::string> hosts = val.second.get_optional<std::string>("presentatie");
		if (hosts.is_initialized())
			program.Hosts(hosts.get());

		boost::optional<std::string> actors = val.second.get_optional<std::string>("acteursnamen_rolverdeling");
		if (actors.is_initialized())
			program.Actors(actors.get());

		boost::optional<std::string> director = val.second.get_optional<std::string>("regisseur");
		if (director.is_initialized())
			program.Director(director.get());
	}

	program.DetailsLoaded(true);
}

std::string TvGidsNL::ConvertGenre(const std::string& genre) const
{
	if (genre == "Nieuws/actualiteiten" || genre == "Actualiteit")
		return "News / Current affairs";
	if (genre == "Amusement" || genre == "Animatie" ||
	    genre == "Comedy")
		return "Show / Game show";
	if (genre == "Jeugd")
		return "Children's / Youth programmes";
	if (genre == "Serie/soap" || genre == "Film" ||
	    genre == "Komedie" || genre == "Actiefilm" ||
	    genre == "Tekenfilm" || genre == "Animatiekomedie" ||
	    genre == "Thriller" || genre == "Familiekomedie" ||
	    genre == "Romantische Komedie" || genre == "Serie" ||
	    genre == "Misdaad")
		return "Movie / Drama";
	if (genre == "Info" || genre == "Informatief" ||
	    genre == "Documentaire" || genre == "Wetenschap" ||
	    genre == "Natuur" || genre == "Educatief")
		return "Education / Science / Factual topics";
	if (genre == "Sport")
		return "Sports";
	if (genre == "Erotiek")
		return "Leisure hobbies";
	if (genre == "Muziek")
		return "Music / Ballet / Dance";
	if (genre == "Religieus")
		return "Social / Political issues / Economics";
	if (genre == "Kunst/Cultuur" || genre == "Kunst/cultuur" || genre == "Cultuur")
		return "Arts / Culture (without music)";
	if (genre == "Overig" || genre == "Overige")
		return "Other";

	return genre;
}

std::string TvGidsNL::ConvertDate(const std::string& date) const
{
	std::stringstream ss;
	ss << boost::regex_replace(date, boost::regex("[-: ]"), "") << " +0100";

	return ss.str();
}


