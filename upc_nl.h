#ifndef UPC_NL_H
#define UPC_NL_H

#include "provider.h"

class UPCNL : public Provider
{
public:
	virtual Channels GetChannels() const;
	virtual Programs GetPrograms(Channels channels, const ScanConfig& scanConfig) const;

private:
	Channels LoadFromJSON(std::string json) const;
	void LoadFromJSON(Programs& programs, Channel channel, std::string json) const;
	std::string ConvertGenre(std::string genre) const;
	std::string ConvertDate(int date) const;
};

#endif // UPC_NL_H

