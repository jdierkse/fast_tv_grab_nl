#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "channels.h"
#include "config.h"
#include "programs.h"
#include "tvgids_nl.h"
#include "upc_nl.h"
#include "functions.h"


struct passwd *pw = getpwuid(getuid());
std::string configFilename = std::string(pw->pw_dir) + "/.xmltv/fast_tv_grab_nl.conf";
std::string cacheFilename = std::string(pw->pw_dir) + "/.xmltv/fast_tv_grab_nl.cache";

void PrintHelp()
{
	std::cout << "Usage: fast_tv_grab_nl [options]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  --provider [upc,tvgids] -p [upc,tvgids] Provider to use" << std::endl;
	std::cout << "  --days N -d N                           Number of days to grab (default: 4)" << std::endl;
	std::cout << "  --quiet -q                              Supress progress output" << std::endl;
	std::cout << "  --clearcache                            Clear the cache file" << std::endl;
	std::cout << "  --createconfig                          Create the configuration file" << std::endl;
	std::cout << "  --help -h                               Print this help information" << std::endl;
}

void ClearCache()
{
	std::ofstream cacheOutFile(cacheFilename.c_str());
}

void CreateConfig(const Provider& provider)
{
	Channels channels = provider.GetChannels();
	ConfigurationFile configFile(configFilename);
	configFile.Channels(channels);
	configFile.Write();
}

std::string GetXML(Configuration configuration, Provider& provider, const ScanConfig& scanConfig)
{
	std::stringstream ss;

	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	ss << "<!DOCTYPE tv SYSTEM \"xmltv.dtd\">" << std::endl;
	ss << "<tv generator-info-name=\"fast_tv_grab_nl\">" << std::endl;

	Channels channels = configuration.Channels();
	ss << channels.GetXML();
	ss << provider.GetPrograms(channels, scanConfig).GetXML();

	ss << "</tv>" << std::endl;

	return ss.str();
}

size_t write_to_buffer(char* data, size_t size, size_t nmemb, std::string* writerData)
{
	if (writerData == NULL)
		return 0;

	writerData->append(data, size * nmemb);
	return size * nmemb;
}

void Test()
{
}

int main(int argc, char** argv)
{
	try
	{
		boost::program_options::options_description description("Options");

		description.add_options()
			("provider,p", boost::program_options::value<std::string>(), "Provider use")
			("days,d", boost::program_options::value<int>(), "Number of days to grab")
			("quiet,q", "Supress progress output")
			("clearcache", "Clear the cache file")
			("createconfig", "Create the configuration file")
			("help,h", "Print this help information")
			("test,t", "Test functionality");

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);

		boost::shared_ptr<Provider> pProvider;
		int days = 4;
		bool quiet = true;

		if (vm.count("days"))
		{
			days = vm["days"].as<int>();
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

		if (vm.count("help"))
		{
			PrintHelp();
			return 0;
		}

		if (vm.count("test"))
		{
			Test();
			return 0;
		}

		if (vm.count("provider"))
		{
			std::string provider = vm["provider"].as<std::string>();
			if (provider == "upc")
			{
				pProvider = boost::make_shared<UPCNL>();
			}
			else if(provider != "tvgids")
			{
				pProvider = boost::make_shared<TvGidsNL>();
			}
			else
			{
				std::cout << "Unknown provider \"" << provider << "\"" << std::endl;
				return -1;
			}
		}
		else
		{
			std::cout << "No provider selected." << std::endl;
			return -1;
		}

		curl_global_init(CURL_GLOBAL_ALL);

		if (vm.count("createconfig"))
		{
			CreateConfig(*pProvider.get());
			return 0;
		}

		ConfigurationFile configFile(configFilename);
		Configuration configuration = configFile.Read(*pProvider.get());

		if (!configuration.Valid())
		{
			std::cout << "No config file present." << std::endl;
			std::cout << "Run fast_tv_grab_nl --createconfig to create a default config file." << std::endl;
			return -1;
		}

		std::cout << GetXML(configuration, *pProvider.get(), ScanConfig(days, quiet, cacheFilename));

		curl_global_cleanup();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
	}

	return 0;
}

