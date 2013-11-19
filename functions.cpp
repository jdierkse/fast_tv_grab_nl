#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include "functions.h"


Channels GetChannels()
{
	HttpData httpData;
	std::string channelsString = httpData.GetUrlContents("http://www.tvgids.nl/json/lists/channels.php");

	Channels channels;
	channels.LoadFromJSON(channelsString);

	return channels;
}

Programs GetPrograms(Channels channels, int days, bool fast, bool quiet)
{
	days = (days > 4)? 4: days;

	Programs programs;

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

	if (!fast)
	{
		int total = 0;
		if (!quiet)
		{
			total = programs.size();
			std::cerr << "Fetching detailed info for " << total << " items:" << std::endl;
		}

		int percent = 0;
		int item = 0;
		for (Programs::iterator it = programs.begin(); it != programs.end(); ++it, ++item)
		{
			HttpData httpData;
			std::stringstream ss;
			ss << "http://www.tvgids.nl/json/lists/program.php?id=" << it->GetId();
			std::string programString = httpData.GetUrlContents(ss.str());

			it->LoadFromJSON(programString);

			if (!quiet)
			{
				int p = (double)item / total * 100;
				if (p != percent)
				{
					std::cerr << p << "%" << std::endl;
					percent = p;
				}
			}
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

