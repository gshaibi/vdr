#include <boost/bind.hpp> // include "this" in thread function  TODO - maybe get read of this and call thread with "this"?

#include <libconfig.h++> // configure ostream and minimal type of log messages

#include <cassert> // asserts
#include <exception> // deals with configuration

#include "logger.hpp" // implementation hpp file
#include "singleton.hpp" // provides access to configurations


namespace ilrd
{

//***************************** INIT GLOBAL VARIABLES **************************

boost::atomic<bool> Logger::s_isReady(false);
boost::atomic<bool> Logger::s_shouldInit(true);
Logger *Logger::s_logger = NULL;
Logger::msgType Logger::s_minimalMsgType = Logger::INFO;
std::ostream *Logger::s_logObject = NULL;

//**************************** GET LOGGER INSTANCE *****************************

Logger *Logger::GetLoggerInstance()
{
    using namespace libconfig;

    if (!s_isReady)
    {
        bool tmpTrue = true;

        if (s_shouldInit.compare_exchange_strong(tmpTrue, false, boost::memory_order_acquire))
        {
            Singleton<Config> cfg;
            
            int minimalMsgType = 0;
            std::ostream *logObject;

            try
            {
                cfg.GetInstance().readFile("../../conf/master.conf");
            }

            catch (const SettingNotFoundException &nfex)
            {
                std::cerr << "Setting '" << nfex.getPath() << 
                                    "' is missing from conf file." << std::endl;
            }

            try // check config for minimal type of message to write
            {
                minimalMsgType = cfg.GetInstance().lookup("LogLevel");
                
                if (minimalMsgType < INFO || minimalMsgType > SPECIAL)
                {
                    minimalMsgType = INFO;
                }
            }

            catch (const SettingNotFoundException &nfex) // couldnt find in config 
            {
                std::cerr << "Setting '" << nfex.getPath() << 
                                    "' is missing from conf file." << std::endl;
            }

            catch (const SettingTypeException &tex) // couldnt convert types
            {
                std::cout << "Setting '" << tex.getPath() << 
                                        "' doesnt match it's type." << std::endl;
            }

            try // check config for stream to write into
            {
                std::string ostreamPath = cfg.GetInstance().lookup("LogStream");

                if ("default" == ostreamPath)
                {
                    //logObject = &std::cerr;
                    logObject = &std::cout;
                }

                else

                {
                    logObject = new std::ofstream(ostreamPath.c_str());
                }
            }

            catch (const SettingNotFoundException &nfex) // couldnt find in config
            {
                std::cerr << "Setting '" << nfex.getPath() << 
                                    "' is missing from conf file." << std::endl;
            }

            catch (const SettingTypeException &tex) // couldnt convert types
            {
                std::cout << "Setting '" << tex.getPath() << 
                                        "' doesnt match it's type." << std::endl;
            }

            s_logger = new Logger(static_cast<msgType>(minimalMsgType), logObject);
            std::atexit(Deleter);
            s_isReady.store(true, boost::memory_order_seq_cst);
            (*s_logObject) << "[Logger] Single logger instance created " << std::endl;
        }
        
        else

        {
            while (!s_isReady.load(boost::memory_order_seq_cst))
            {
                boost::this_thread::yield();
            }
        }

    }

    return s_logger;
}

//********************************* CTOR ***************************************

Logger::Logger(msgType type, std::ostream *logObject): m_Q()
{
    s_minimalMsgType = type;
    s_logObject = logObject;

    // create thread
   m_thread = new boost::thread(boost::bind(&Logger::ThreadReadFromQ, this));
   (*s_logObject) << "[Logger] Ctor()" << std::endl;
}

//********************************* DTOR ***************************************

Logger::~Logger()
{
    //add termination task. the scoped thread will close itself
    std::pair<char *, msgType> terminator("[Logger] terminate thread request", TERM);
    m_Q.Enqueue(terminator);
    m_thread->join();
    (*s_logObject) << "[Logger] Dtor()" << std::endl;
}

//********************************** LOG ***************************************

void Logger::WriteLog(const char *msg, msgType type)
{
    assert(type > TERM && type <= SPECIAL); // TERM is sent only at Dtor

    if (type >= s_minimalMsgType)
    {
        (*s_logObject) << "[Logger] WriteLog()" << std::endl;
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
            (*s_logObject) << "[Logger] Termination Signal recieved by thread" << std::endl;
            return;
        }

        (*s_logObject) << "Message Type: " << msgToWrite.second << " Message: " 
                                               << msgToWrite.first << std::endl;
    }
}

} // namespace ilrd
