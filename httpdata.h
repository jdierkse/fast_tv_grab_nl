#ifndef HTTPDATA_H
#define HTTPDATA_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <openssl/crypto.h>
#include <curl/curl.h>


static std::vector<boost::shared_ptr<boost::mutex> > g_mutexes;

static void lock_callback(int mode, int type, const char *file, int line)
{
	if (mode & CRYPTO_LOCK)
		g_mutexes[type]->lock();
	else
		g_mutexes[type]->unlock();
}
 
static unsigned long thread_id(void)
{
	return (unsigned long)pthread_self();
}
 
static void initialize_locks(void)
{
	g_mutexes.resize(CRYPTO_num_locks());
	for (int i = 0; i < CRYPTO_num_locks(); ++i)
		g_mutexes[i] = boost::shared_ptr<boost::mutex>(new boost::mutex());

	CRYPTO_set_id_callback(thread_id);
	CRYPTO_set_locking_callback(lock_callback);
}
 
static void free_locks(void)
{
	CRYPTO_set_locking_callback(NULL);
}

class HttpData
{
public:
	HttpData();
	~HttpData();

	std::string GetUrlContents(const std::string& url);

private:
	static size_t WriteCallback(char* data, size_t size, size_t nmemb, std::string* writerData);

private:
	std::vector<std::string> m_userAgents;
};

#endif // HTTPDATA_H

