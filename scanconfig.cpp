#include "scanconfig.h"

ScanConfig::ScanConfig(int days, bool fast, bool quiet, bool cache, std::string cacheFilename) :
	days(days),
	fast(fast),
	quiet(quiet),
	cache(cache),
	cacheFilename(cacheFilename)
{
}

