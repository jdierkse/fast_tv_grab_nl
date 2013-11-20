#include <fstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "functions.h"


Channels GetChannels()
{
	HttpData httpData;
	std::string channelsString = httpData.GetUrlContents("http://www.tvgids.nl/json/lists/channels.php");

	Channels channels;
	channels.LoadFromJSON(channelsString);
	std::sort(channels.begin(), channels.end());

	return channels;
}

Programs GetPrograms(Channels channels, int days, bool fast, bool quiet, bool cache, std::string cacheFilename)
{
	days = (days > 4)? 4: days;

	Programs programs;
	if (cache)
	{
		std::ifstream cacheInFile(cacheFilename.c_str());
		if (cacheInFile.good())
		{
			boost::archive::text_iarchive cacheInArchive(cacheInFile);
			cacheInArchive >> programs;
		}
	}

	if (!quiet)
		std::cerr << "Fetching basic information for " << channels.size() << " channels:" << std::endl;

	for (Channels::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (!quiet)
			std::cerr << it->GetName() << std::endl;

		for (int day = 1; day <= days; ++day)
		{
			HttpData httpData;
			std::stringstream ss;
			ss << "http://www.tvgids.nl/json/lists/programs.php?channels=" << it->GetId() << "&day=" << day;
			std::string programsString = httpData.GetUrlContents(ss.str());

			programs.LoadFromJSON(*it, programsString);
		}
	}

	if (cache)
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
			if (!quiet)
			{
				std::cerr << "Removing " << total << " old items from cache" << std::endl;
				std::cerr << " 0%";
			}
	
			// TODO: make function
			int percent = 0;
			int item = 0;
			for (std::vector<Program>::iterator it = deleteList.begin(); it != deleteList.end(); ++it, ++item)
			{
				programs.Remove(*it);

				if (!quiet)
				{
					int p = (double)item / total * 100;
					if (p != percent)
					{
						std::cerr << '\b' << '\b' << '\b';

						if (p < 10)
							std::cerr << " ";

						std::cerr << p << "%";
						percent = p;
					}
				}
			}

			if (!quiet)
			{
				std::cerr << '\b' << '\b' << '\b';
				std::cerr << "100%" << std::endl;
			}
		}
	}

	if (!fast)
	{
		int total = 0;
		if (!quiet)
		{
			total = programs.size();
			std::cerr << "Fetching detailed info for " << total << " items:" << std::endl;
			std::cerr << " 0%";
		}

		int percent = 0;
		int item = 0;
		for (Programs::iterator it = programs.begin(); it != programs.end(); ++it, ++item)
		{
			if (!it->GetDetailsLoaded())
			{
				HttpData httpData;
				std::stringstream ss;
				ss << "http://www.tvgids.nl/json/lists/program.php?id=" << it->GetId();
				std::string programString = httpData.GetUrlContents(ss.str());

				it->LoadDetailsFromJSON(programString);
			}

			// TODO: make function
			if (!quiet)
			{
				int p = (double)item / total * 100;
				if (p != percent)
				{
					std::cerr << '\b' << '\b' << '\b';

					if (p < 10)
						std::cerr << " ";

					std::cerr << p << "%";
					percent = p;
				}
			}
		}
	}

	if (!quiet)
	{
		std::cerr << '\b' << '\b' << '\b';
		std::cerr << "100%" << std::endl;
	}

	std::sort(programs.begin(), programs.end());

	if (cache)
	{
		std::ofstream cacheOutFile(cacheFilename.c_str());
		if (cacheOutFile.good())
		{
			boost::archive::text_oarchive cacheOutArchive(cacheOutFile);
			cacheOutArchive << programs;
		}
	}

	return programs;
}

std::vector<std::string> SplitString(std::string string, std::string delimiter)
{
	std::vector<std::string> strings;
	boost::algorithm::split_regex(strings, string, boost::regex(delimiter));
	return strings;
}

