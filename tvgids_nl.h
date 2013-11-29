#ifndef TVGIDS_NL_H
#define TVGIDS_NL_H

#include "provider.h"

class TvGidsNL : public Provider
{
public:
	virtual Channels GetChannels() const;
	virtual Programs GetPrograms(Channels channels, const ScanConfig& scanConfig) const;

private:
	Channels LoadFromJSON(std::string json) const;
	void LoadFromJSON(Programs& programs, Channel channel, std::string json) const;
	void LoadDetailsFromJSON(Program& program, std::string json) const;
};

#endif // TVGIDS_NL_H

