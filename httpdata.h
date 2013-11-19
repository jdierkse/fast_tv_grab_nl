#ifndef HTTPDATA_H
#define HTTPDATA_H

#include <string>
#include <vector>
#include <curl/curl.h>


static std::string g_http_data;

class HttpData
{
public:
	HttpData();
	~HttpData();

	std::string GetUrlContents(std::string url);

private:
	static size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);

private:
	CURL* m_curl;
	std::vector<std::string> m_userAgents;
};

#endif // HTTPDATA_H

