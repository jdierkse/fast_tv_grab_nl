#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "channels.h"
#include "provider.h"


class Configuration
{
public:
	friend class ConfigurationFile;
	Configuration();

	bool Valid() const;
	::Channels Channels() const;

protected:
	void Valid(bool valid);
	void Channels(const ::Channels& channels);

private:
	bool m_valid;
	::Channels m_channels;
};

class ConfigurationFile
{
public:
	ConfigurationFile(const std::string& filename);

public:
	void Channels(const ::Channels& channels);

	Configuration Read(const Provider& provider);
	void Write();

private:
	std::string m_filename;
	Configuration m_configuration;
};

#endif // CONFIG_H

