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

std::vector<std::string> SplitString(const std::string& string, const std::string& delimiter)
{
	std::vector<std::string> strings;
	boost::algorithm::split_regex(strings, string, boost::regex(delimiter));
	return strings;
}

std::string FixHTMLAndSpecialCharacters(const std::string& string)
{
	std::string str = boost::regex_replace(string, boost::regex("<[a-zA-Z1-9\\/ ]*>"), "");
	str = boost::regex_replace(str, boost::regex("\r"), "");
	str = boost::regex_replace(str, boost::regex("&"), "&amp;");
	str = boost::regex_replace(str, boost::regex("<"), "&lt;");
	str = boost::regex_replace(str, boost::regex(">"), "&gt;");
	return str;
}

