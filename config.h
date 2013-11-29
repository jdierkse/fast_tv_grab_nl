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

	bool GetValid() const;
	Channels GetChannels() const;

protected:
	void SetValid(bool valid);
	void SetChannels(Channels channels);

private:
	bool m_valid;
	Channels m_channels;
};

class ConfigurationFile
{
public:
	ConfigurationFile(std::string filename);

public:
	void SetChannels(Channels channels);

	Configuration Read(const Provider& provider);
	void Write();

private:
	std::string m_filename;
	Configuration m_configuration;
};

#endif // CONFIG_H

