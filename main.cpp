#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <boost/program_options.hpp>
#include "httpdata.h"
#include "channels.h"
#include "config.h"
#include "programs.h"
#include "functions.h"


struct passwd *pw = getpwuid(getuid());
std::string configFilename = std::string(pw->pw_dir) + "/.xmltv/fast_tv_grab_nl.conf";
std::string cacheFilename = std::string(pw->pw_dir) + "/.xmltv/fast_tv_grab_nl.cache";

void PrintHelp()
{
	std::cout << "Usage: fast_tv_grab_nl [options]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  --days N -d N   Number of days to grab (default: 4)" << std::endl;
	std::cout << "  --fast -f       Don't grab detailed information" << std::endl;
	std::cout << "  --nocache -n    Don't use caching" << std::endl;
	std::cout << "  --quiet -q      Supress progress output" << std::endl;
	std::cout << "  --clearcache    Clear the cache file" << std::endl;
	std::cout << "  --createconfig  Create the configuration file" << std::endl;
	std::cout << "  --help -h       Print this help information" << std::endl;
}

void ClearCache()
{
	std::ofstream cacheOutFile(cacheFilename.c_str());
}

void CreateConfig()
{
	Channels channels = GetChannels();
	ConfigurationFile configFile(configFilename);
	configFile.SetChannels(channels);
	configFile.Write();
}

std::string GetXML(Configuration configuration, int days, bool fast, bool quiet, bool cache, std::string cacheFilename)
{
	std::stringstream ss;

	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	ss << "<!DOCTYPE tv SYSTEM \"xmltv.dtd\">" << std::endl;
	ss << "<tv generator-info-name=\"fast_tv_grab_nl\">" << std::endl;

	Channels channels = configuration.GetChannels();
	ss << channels.GetXML();
	ss << GetPrograms(channels, days, fast, quiet, cache, cacheFilename).GetXML();

	ss << "</tv>" << std::endl;

	return ss.str();
}

int main(int argc, char** argv)
{
	try
	{
		boost::program_options::options_description description("Options");

		description.add_options()
			("days,d", boost::program_options::value<int>(), "Number of days to grab")
			("fast,f", "Don't grab detailed information")
			("nocache,n", "Don't use caching")
			("quiet,q", "Supress progress output")
			("clearcache", "Clear the cache file")
			("createconfig", "Create the configuration file")
			("help,h", "Print this help information");

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);

		// TODO: Multithreading

		int days = 4;
		bool fast = false;
		bool quiet = false;
		bool cache = true;

		if (vm.count("days"))
		{
			days = vm["days"].as<int>();
		}

		if (vm.count("fast"))
		{
			fast = true;
		}

		if (vm.count("nocache"))
		{
			cache = false;
		}

		if (vm.count("quiet"))
		{
			quiet = true;
		}

		if (vm.count("clearcache"))
		{
			ClearCache();
			return 0;
		}

		if (vm.count("createconfig"))
		{
			CreateConfig();
			return 0;
		}

		if (vm.count("help"))
		{
			PrintHelp();
			return 0;
		}

		ConfigurationFile configFile(configFilename);
		Configuration configuration = configFile.Read();
		if (!configuration.GetValid())
		{
			std::cout << "No config file present." << std::endl;
			std::cout << "Run fast_tv_grab_nl --createconfig to create a default config file." << std::endl;
			return -1;
		}

		std::cout << GetXML(configuration, days, fast, quiet, cache, cacheFilename);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
	}

	return 0;
}

