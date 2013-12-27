#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

class ThreadPool
{
public:

	ThreadPool(std::size_t poolSize) :
		m_running(true)
	{
		for (std::size_t i = 0; i < poolSize; ++i)
			m_threads.create_thread(boost::bind(&ThreadPool::ThreadMain, this));
	}

	~ThreadPool()
	{
		try
		{
			Join();
		}
		catch (...)
		{
		}
	}

	template <typename Task>
	void Execute(Task task)
	{
		boost::unique_lock<boost::mutex> lock(m_startMutex);
		m_tasks.push(boost::function<void()>(task));
		m_startCondition.notify_one();
	}

	void Join()
	{
		boost::unique_lock<boost::mutex> lock(m_endMutex);
		while (!m_tasks.empty())
			m_endCondition.wait(lock);

		{
			boost::unique_lock<boost::mutex> lock(m_startMutex);
			m_running = false;
			m_startCondition.notify_all();
		}

		m_threads.join_all();

		{
			boost::unique_lock<boost::mutex> lock(m_startMutex);
			m_running = true;
		}
	}

private:
	void ThreadMain()
	{
		while(m_running)
		{
			boost::unique_lock<boost::mutex> lock(m_startMutex);
			while (m_tasks.empty() && m_running)
				m_startCondition.wait(lock);

			if (m_running)
			{
				boost::function<void()> task = m_tasks.front();
				m_tasks.pop();

				lock.unlock();

				try
				{
					task();
				}
				catch (...)
				{
				}
			}

			if (!m_running)
				break;

			boost::unique_lock<boost::mutex> l(m_endMutex);
			m_endCondition.notify_all();
			lock.lock();
		}
	}

private:
	std::queue<boost::function<void()> > m_tasks;
	boost::thread_group m_threads;

	boost::mutex m_startMutex;
	boost::condition_variable m_startCondition;

	boost::mutex m_endMutex;
	boost::condition_variable m_endCondition;

	bool m_running;
};

#endif // THREADPOOL_H

