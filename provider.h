#ifndef PROVIDER_H
#define PROVIDER_H

#include <string>
#include "scanconfig.h"
#include "channels.h"
#include "programs.h"


class Provider
{
public:
	virtual Channels GetChannels() const = 0;
	virtual Programs GetPrograms(Channels channels, const ScanConfig& scanConfig) const = 0;

};

#endif // PROVIDER_H

