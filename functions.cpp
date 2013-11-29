#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include "functions.h"


int OutputProgress(int currentItem, int totalItems, int previousPercentage)
{
	if (currentItem == 0)
	{
		std::cerr << "  0%";
		return 0;
	}

	int currentPercentage = (double)currentItem / totalItems * 100;
	if (currentPercentage != previousPercentage)
	{
		std::cerr << '\b' << '\b' << '\b' << '\b';

		if (currentPercentage < 10)
			std::cerr << "  ";
		else if (currentPercentage < 100)
			std::cerr << " ";

		std::cerr << currentPercentage << "%";
	}

	if (currentItem == totalItems)
		std::cerr << std::endl;

	return currentPercentage;
}

std::vector<std::string> SplitString(std::string string, std::string delimiter)
{
	std::vector<std::string> strings;
	boost::algorithm::split_regex(strings, string, boost::regex(delimiter));
	return strings;
}

std::string FixHTMLAndSpecialCharacters(std::string string)
{
	string = boost::regex_replace(string, boost::regex("<[a-zA-Z1-9\\/ ]*>"), "");
	string = boost::regex_replace(string, boost::regex("\r"), "");
	string = boost::regex_replace(string, boost::regex("&"), "&amp;");
	return string;
}

