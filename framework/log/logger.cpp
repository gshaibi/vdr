#include <boost/chrono.hpp> // timed join in dtor
#include <boost/thread/locks.hpp> // unique lock
#include <boost/bind.hpp> // include "this" in thread function

#include <pthread.h> // forcefully cancel thread in dtor
#include <cassert> // asserts

#include "logger.hpp" // implementation hpp file
#include "singleton.hpp" // provides access to configurations
#include "config.hpp" // configure ostream and minimal type of log messages

namespace ilrd
{

//**************************** GET LOGGER INSTANCE *****************************

Logger *GetLoggerInstance(msgType type, std::ostream logObject)
{
    bool tmpTrue = true;

    if (!Logger::s_isReady)
    {

        if (Logger::s_shouldInit.compare_exchange_strong(tmpTrue, false, boost::memory_order_acquire))
        {
            std::ostream logObject;

            Logger::msgType minimalMsgType = config;

            if (type < INFO || type > SPECIAL)
            {
                minimalMsgType = Logger::INFO;
            }

            std::string ostreamPath(config);

            if ("default" == config)
            {
                logObject = std::cerr;
            }

            else

            {
                logObject(ostreamPath.c_str());
            }

            Logger::s_logger = new Logger(minimalMsgType, stream);
            std::atexit(Logger::Deleter);
            Logger::s_isReady.store(true, boost::memory_order_seq_cst);
        }
        
        else

        {
            while (!Logger::s_isReady.load(boost::memory_order_seq_cst))
            {
                boost::this_thread::yield();
            }
        }

    }

    return s_logger;
}

//**************************** GENERAL LOG FUNCTION ****************************

void Log(std::string s_, int typeMsg)
{
    Logger *logger = Logger::GetLoggerInstance();
    logger->Log(s_.c_str(), static_cast<Logger::msgType>(typeMsg));
}

//********************************* CTOR ***************************************

Logger::Logger(msgType type, std::ostream &logObject): m_Q(),
                                                       m_minimalMsgType(type),
                                                       m_logObject(logObject)
{
    // create thread
    boost::scoped_thread<> executor(&Logger::ThreadReadFromQ, this); // joins itself at exit
    m_logObject << "[Logger] Ctor" << std::endl;
}

//********************************* DTOR ***************************************

Logger::~Logger()
{
    //add termination task. the scoped thread will close itself
    std::pair<char *, msgType> terminator("[Logger] terminate thread request", TERM);
    m_Q.Enqueue(terminator);
}

//********************************** LOG ***************************************

void Logger::Log(const char *msg, msgType type)
{
    assert(type > TERM && type <= SPECIAL); // TERM is sent only at Dtor

    if (type >= m_minimalMsgType)
    {
        Message itemToInsert(const_cast<char *>(msg), type);
        m_Q.Enqueue(itemToInsert);
    }
}

//*************************** THREAD READ FROM Q *******************************

void Logger::ThreadReadFromQ()
{
    Message msgToWrite;

    while (1)
    {
        m_Q.Dequeue(msgToWrite);

        if (TERM == msgToWrite.second) // TERM sent at Dtor
        {
            return;
        }

        m_logObject << "Message Type: " << msgToWrite.second << " Message: " 
                                               << msgToWrite.first << std::endl;
    }
}

} // namespace ilrd
