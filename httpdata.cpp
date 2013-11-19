#include "httpdata.h"


HttpData::HttpData()
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();

	m_userAgents.push_back("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)");
	m_userAgents.push_back("Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.9) Gecko/20071025 Firefox/2.0.0.9");
	m_userAgents.push_back("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322)");
	m_userAgents.push_back("Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.7) Gecko/20060909 Firefox/1.5.0.7");
	m_userAgents.push_back("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)");
	m_userAgents.push_back("Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.9) Gecko/20071105 Firefox/2.0.0.9");
	m_userAgents.push_back("Mozilla/5.0 (Macintosh; U; Intel Mac OS X; en-US; rv:1.8.1.9) Gecko/20071025 Firefox/2.0.0.9");
	m_userAgents.push_back("Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.8) Gecko/20071022 Ubuntu/7.10 (gutsy) Firefox/2.0.0.8");

	m_userAgents.push_back("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36");
	m_userAgents.push_back("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/27.0.1453.116 Safari/537.36");
	m_userAgents.push_back("Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.57 Safari/537.36");
	m_userAgents.push_back("Mozilla/5.0 (X11; Linux i686) AppleWebKit/534.34 (KHTML, like Gecko) Qt/4.8.4 Safari/534.34");
	m_userAgents.push_back("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0;  WOW64;  Trident/5.0)");
	m_userAgents.push_back("Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:14.0; ips-agent) Gecko/20100101 Firefox/14.0.1");
	m_userAgents.push_back("Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:25.0) Gecko/20100101 Firefox/25.0");
	m_userAgents.push_back("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.0.04506.648; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET4.0C; .NET4.0E)");
	m_userAgents.push_back("Mozilla/4.0 (compatible; MSIE 6.1; Windows NT)");
}

HttpData::~HttpData()
{
	curl_easy_cleanup(m_curl);
	curl_global_cleanup();
}

std::string HttpData::GetUrlContents(std::string url)
{
	g_http_data.clear();

	curl_easy_setopt(m_curl, CURLOPT_USERAGENT, m_userAgents[rand() % m_userAgents.size()].c_str());
	curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &writeCallback);

	curl_easy_perform(m_curl);

	return g_http_data;
}

size_t HttpData::writeCallback(char* buf, size_t size, size_t nmemb, void* up)
{
	for (int i = 0; i < size * nmemb; i++)
		g_http_data.push_back(buf[i]);

	return size * nmemb;
}

