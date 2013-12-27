#ifndef SCANCONFIG_H
#define SCANCONFIG_H

#include <string>


struct ScanConfig
{
public:
	ScanConfig(int days, bool quiet, const std::string& cacheFilename);

	int days;
	bool quiet;
	std::string cacheFilename;
};

#endif // SCANCONFIG_H

