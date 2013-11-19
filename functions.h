#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <string>
#include <vector>
#include "httpdata.h"
#include "channels.h"
#include "programs.h"


Channels GetChannels();
Programs GetPrograms(Channels channels, int days, bool fast, bool quiet, bool cache, std::string cacheFilename);
std::vector<std::string> SplitString(std::string string, std::string delimiter);

#endif // FUNCTIONS_H

