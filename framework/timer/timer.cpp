#include <iostream>
#include <cassert>
#include <boost/bind.hpp>   // to wrap the callback
#include <sys/timerfd.h>    // timerfd

#include "routines.hpp"
#include "timer.hpp"

namespace ilrd
{

Timer::Timer(Reactor& r_) : m_handleCounter(0),
                            m_reactor(r_), 
                            m_callBacks(),
                            m_timerFd(new int(timerfd_create(CLOCK_MONOTONIC, 0)), CloseFD)
{ 
    if (-1 == *m_timerFd)
    {
        std::cout << "timer_create() failed\n";
    }
}

Timer::Handle Timer::Set(Duration& duration_, 
                        CallBack callback_)
{
    ilrd::Log("Timer::Set()");
    assert(!callback_.empty());
    
    TimePoint real_time = boost::chrono::steady_clock::now() + duration_;
    m_callBacks[real_time] = std::make_pair(m_handleCounter, callback_);

    if (1 == m_callBacks.size())
    {
        m_reactor.AddFD(*m_timerFd, Reactor::READ, 
            boost::bind(&Timer::CallBackWrapper, this));
        SetTimerIMP(duration_);
    }
    else
    {
       if (real_time == m_callBacks.begin()->first) // new timer is sooner than current
        {
            ilrd::Log("Timer::Set() shorter time was set");
            SetTimerIMP(duration_);
        }
    } 
    
    return (m_handleCounter++);
}

void Timer::SetTimerIMP(Duration& duration_)
{
    ilrd::Log("Timer::SetTimerIMP()");
    std::cout << duration_ << " " << duration_.count() << std::endl;

    struct timespec nsec = {0, duration_.count()};      // {sec, nsec}
    struct itimerspec new_timeout = { {0, 0}, nsec};    // {interval, time_value}

    if (-1 == timerfd_settime(*m_timerFd, 0, &new_timeout, NULL))
    {
        ilrd::Log("Timer::SetTimerIMP timerfd_settime() failed-------------------------XX");
        //TODO: throw runtime_error(std::perror)
    }
}

void Timer::CancelTimerIMP()
{
    struct itimerspec new_timeout = { {0, 0}, {0, 0}};
    if (-1 == timerfd_settime(*m_timerFd, 0, &new_timeout, NULL))
    {
        ilrd::Log("Timer::SetTimerIMP timerfd_settime() failed---------XX");
        //TODO: throw runtime_error(std::perror)
    }
}

void Timer::Cancel(Handle handle_)
{    
    ilrd::Log("Timer::Cancel()");
    assert(!m_callBacks.empty());
    
    std::map<TimePoint, std::pair<Handle, CallBack> >::iterator curr = m_callBacks.begin();
    
    if (handle_ == curr->second.first) // if it is the handle of the current timer
    {
        m_callBacks.erase(curr++);
        if (curr == m_callBacks.end())
        {
            ilrd::Log("cancel last");
            CancelTimerIMP();
            m_reactor.RemFD(*m_timerFd, Reactor::READ);
            return;
        }

        Duration new_dur = curr->first - boost::chrono::steady_clock::now();
        SetTimerIMP(new_dur);
        return;
    }
    
    while (++curr != m_callBacks.end())
    {
        if (handle_ == curr->second.first)
        {
            m_callBacks.erase(curr);
            break;
        }
    }
}

void Timer::CallBackWrapper()
{
    ilrd::Log("Timer::CallBackWrapper()");

    m_callBacks.begin()->second.second(); // callback()

    std::map<TimePoint, std::pair<Handle, CallBack> >::iterator curr = m_callBacks.begin();
    m_callBacks.erase(curr++);

    if (m_callBacks.empty())
    {
        CancelTimerIMP();
        m_reactor.RemFD(*m_timerFd, Reactor::READ);
    }
    else
    {
        Duration next_dur = curr->first - boost::chrono::steady_clock::now();
        SetTimerIMP(next_dur);
    }
}

void Timer::CloseFD(int* fd_)
{
    close(*fd_);
    delete fd_;
}

} // ilrd