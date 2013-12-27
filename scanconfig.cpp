#include "scanconfig.h"

ScanConfig::ScanConfig(int days, bool quiet, const std::string& cacheFilename) :
	days(days),
	quiet(quiet),
	cacheFilename(cacheFilename)
{
}

