#ifndef LOGGER
#define LOGGER

#include <fstream> // log file manipulations
#include <iostream> // std::cerr
#include <queue> // waitable queue is based on simple queue

#include <boost/atomic.hpp> // atomic variables for the singleton menagement
#include <boost/thread/scoped_thread.hpp> 
#include <boost/noncopyable.hpp> // logger class is singleton and noncopyable
#include <boost/thread.hpp> // thread object of the logger

#include "waitable_q.hpp" // queue used to store log messages until written

namespace ilrd
{

class Logger: boost::noncopyable
{

public:

	enum msgType {
		TERM = 0,
		INFO = 1,
		WARNING = 2,
		ERROR = 3,
		SPECIAL = 4
	};

	~Logger(); // thread safe closure

	void WriteLog(const char *msg, msgType type);
	static Logger *GetLoggerInstance();

private:

	typedef std::pair<char *, msgType> Message;

	Logger(msgType type = INFO, std::ostream *logObject = &std::cerr);
	void ThreadReadFromQ(); // thread will use it to read from the Q
	inline static void Deleter();

	static Logger *s_logger;
	static boost::atomic<bool> s_shouldInit;
	static boost::atomic<bool> s_isReady;

	WaitableQ<Message> m_Q;
	boost::thread *m_thread;
	static msgType s_minimalMsgType; // the minimal value of message type to write to log
	static std::ostream *s_logObject; // where the logs will be written to
};

//******************************* LOGGER DELETER *******************************

inline void Logger::Deleter()
{
	(*s_logObject) << "[Logger] Deleter()" << std::endl;
	delete s_logger;
	s_logger = NULL;

	if (s_logObject != &std::cerr)
	{
		static_cast<std::ofstream *>(s_logObject)->close();
	}

}

//**************************** GENERAL LOG FUNCTION ****************************

inline void Log(std::string s_, int typeMsg)
{
	Logger *logger = Logger::GetLoggerInstance();
	logger->WriteLog(s_.c_str(), static_cast<Logger::msgType>(typeMsg));
}

}//namespace ilrd

#endif // LOGGER



