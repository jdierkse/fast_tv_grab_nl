#ifndef SCANCONFIG_H
#define SCANCONFIG_H

#include <string>


struct ScanConfig
{
public:
	ScanConfig(int days, bool fast, bool quiet, bool cache, std::string cacheFilename);

	int days;
	bool fast;
	bool quiet;
	bool cache;
	std::string cacheFilename;
};

#endif // SCANCONFIG_H

