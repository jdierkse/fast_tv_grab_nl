#ifndef UPC_NL_H
#define UPC_NL_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include "httpdata.h"
#include "threadpool.h"
#include "provider.h"

class UPCNL : public Provider
{
public:
	UPCNL();

	virtual Channels GetChannels() const;
	virtual Programs GetPrograms(Channels& channels, const ScanConfig& scanConfig);

private:
	void GetChannelPrograms(Channel& channel, const boost::shared_ptr<HttpData>& pHttpData, const ScanConfig& scanConfig, int item, int total);
	Channels LoadFromJSON(const std::string& json) const;
	void LoadFromJSON(Programs& programs, const Channel& channel, const std::string& json) const;
	std::string ConvertGenre(const std::string& genre) const;
	std::string ConvertDate(int date) const;

private:
	ThreadPool m_threadPool;
	boost::mutex m_mutex;
	std::vector<std::pair<Channel, std::string> > m_programs;
};

#endif // UPC_NL_H

