#include <fstream>
#include <map>
#include <boost/filesystem.hpp>
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

bool Configuration::Valid() const
{
	return m_valid;
}

::Channels Configuration::Channels() const
{
	return m_channels;
}

void Configuration::Valid(bool valid)
{
	m_valid = valid;
}

void Configuration::Channels(const ::Channels& channels)
{
	m_channels = channels;
}

ConfigurationFile::ConfigurationFile(const std::string& filename) :
	m_filename(filename)
{
}

void ConfigurationFile::Channels(const ::Channels& channels)
{
	m_configuration.Channels(channels);
}

Configuration ConfigurationFile::Read(const Provider& provider)
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

	::Channels channels;
	::Channels allChannels = provider.GetChannels();
	
	for (::Channels::iterator it = allChannels.begin(); it != allChannels.end(); ++it)
	{
		if (channelMap.find(it->Id()) != channelMap.end())
		{
			it->Name(channelMap[it->Id()]);
			channels.push_back(*it);
		}
	}

	m_configuration.Channels(channels);
	m_configuration.Valid(true);

	return m_configuration;
}

void ConfigurationFile::Write()
{
	boost::filesystem::path fullpath(m_filename);
	boost::filesystem::path directory = fullpath.parent_path();
	boost::filesystem::create_directories(directory);

	std::ofstream file(m_filename.c_str());
	::Channels channels = m_configuration.Channels();
	for (::Channels::iterator it = channels.begin(); it != channels.end(); ++it)
		file << it->Id() << "=" << it->Name() << std::endl;
}

