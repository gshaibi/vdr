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

            // NOTE: this part is for testing only and should stay commented elsewhere
            
            // try
            // {
            //     cfg.GetInstance().readFile("../../conf/master.conf");
            // }

            // catch (const SettingNotFoundException &nfex)
            // {
            //     std::cerr << "Setting '" << nfex.getPath() << 
            //                         "' is missing from conf file." << std::endl;
            // }

            try // check config for minimal type of message to write
            {
                minimalMsgType = cfg.GetInstance().lookup("LogLevel");

                if (minimalMsgType < LogMsgTypes::INFO || minimalMsgType > LogMsgTypes::SPECIAL)
                {
                    minimalMsgType = LogMsgTypes::INFO;
                }
            }

            catch (const SettingNotFoundException &nfex) // couldnt find in config 
            {
                std::cerr << "Setting '" << nfex.getPath() << 
                                    "' is missing from conf file." << std::endl;
            }

            catch (const SettingTypeException &tex) // couldnt convert types
            {
                std::cerr << "Setting '" << tex.getPath() << 
                                        "' doesnt match it's type." << std::endl;
            }

            try // check config for stream to write into
            {
                std::string ostreamPath = cfg.GetInstance().lookup("LogStream");

                if ("default" == ostreamPath)
                {
                    logObject = &std::cerr;
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
                std::cerr << "Setting '" << tex.getPath() << 
                                        "' doesnt match it's type." << std::endl;
            }

            catch (...)
            {
                throw;
            }

            s_logger = new Logger(static_cast<LogMsgTypes::msgType>(minimalMsgType), logObject);
            std::atexit(Deleter);
            s_isReady.store(true, boost::memory_order_seq_cst);
            (*(s_logger->m_logObject)) << "[Logger] Single logger instance created " << std::endl;
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

Logger::Logger(LogMsgTypes::msgType type, std::ostream *logObject) : m_messages(),
                                                                     m_minimalMsgType(type),
                                                                     m_logObject(logObject),
                                                                     m_messageWriter(boost::bind(&Logger::ThreadReadFromQ, this))
{
   (*m_logObject) << "[Logger] Ctor()" << std::endl;
}

//********************************* DTOR ***************************************

Logger::~Logger()
{
    //add termination task. the scoped thread will close itself
    Message terminator("[Logger] terminate thread request", LogMsgTypes::TERM);
    m_messages.Enqueue(terminator);
    m_messageWriter.join();

    (*m_logObject) << "[Logger] Dtor()" << std::endl;

    if (m_logObject != &std::cerr)
    {
        static_cast<std::ofstream *>(m_logObject)->close();
        delete m_logObject;
    }
}

//********************************** LOG ***************************************

void Logger::WriteLog(const char *msg, LogMsgTypes::msgType type)
{
    assert(type > LogMsgTypes::TERM && type <= LogMsgTypes::SPECIAL); // TERM is sent only at Dtor

    if (type >= m_minimalMsgType)
    {
        (*m_logObject) << "[Logger] WriteLog()" << std::endl;
        Message itemToInsert(const_cast<char *>(msg), type);
        m_messages.Enqueue(itemToInsert);
    }
}

//*************************** THREAD READ FROM Q *******************************

void Logger::ThreadReadFromQ()
{
    Message msgToWrite;

    while (1)
    {
        m_messages.Dequeue(msgToWrite);

        if (LogMsgTypes::TERM == msgToWrite.second) // TERM sent at Dtor
        {
            (*m_logObject) << "[Logger] Termination Signal recieved by thread" << std::endl;
            return;
        }

        (*m_logObject) << "Message Type: " << msgToWrite.second << " Message: " 
                                               << msgToWrite.first << std::endl;
    }
}

} // namespace ilrd
