#ifndef TVGIDS_NL_H
#define TVGIDS_NL_H

#include "provider.h"

class TvGidsNL : public Provider
{
public:
	virtual Channels GetChannels() const;
	virtual Programs GetPrograms(Channels& channels, const ScanConfig& scanConfig);

private:
	Channels LoadFromJSON(const std::string& json) const;
	void LoadFromJSON(Programs& programs, const Channel& channel, const std::string& json) const;
	std::string ConvertGenre(int genre) const;
	std::string ConvertDate(int date) const;
};

#endif // TVGIDS_NL_H
