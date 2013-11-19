#include <fstream>
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "functions.h"
#include "config.h"


bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

Configuration::Configuration() :
	m_valid(false)
{
}

bool Configuration::GetValid()
{
	return m_valid;
}

Channels Configuration::GetChannels()
{
	return m_channels;
}

void Configuration::SetValid(bool valid)
{
	m_valid = valid;
}

void Configuration::SetChannels(Channels channels)
{
	m_channels = channels;
}

ConfigurationFile::ConfigurationFile(std::string filename) :
	m_filename(filename)
{
}

void ConfigurationFile::SetChannels(Channels channels)
{
	m_configuration.SetChannels(channels);
}

Configuration ConfigurationFile::Read()
{
	std::ifstream file(m_filename.c_str());
	if (!file.good())
		return Configuration();

	std::map<int, std::string> channelMap;

	std::string line;
	while (std::getline(file, line))
	{
		std::vector<std::string> split;
		boost::split(split, line, boost::is_any_of("="));

		if (is_number(split[0]))
			channelMap[boost::lexical_cast<int>(split[0])] = split[1];
	}

	Channels channels;
	Channels allChannels = GetChannels();
	
	for (Channels::iterator it = allChannels.begin(); it != allChannels.end(); ++it)
	{
		if (channelMap.find(it->GetId()) != channelMap.end())
			channels.push_back(*it);
	}

	m_configuration.SetChannels(channels);
	m_configuration.SetValid(true);

	return m_configuration;
}

void ConfigurationFile::Write()
{
	std::ofstream file(m_filename.c_str());
	Channels channels = m_configuration.GetChannels();
	for (Channels::iterator it = channels.begin(); it != channels.end(); ++it)
		file << it->GetId() << "=" << it->GetName() << std::endl;
}

