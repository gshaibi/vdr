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
                            m_timerFd(timerfd_create(CLOCK_MONOTONIC, 0)),
                            m_callBacks()
{ 
    if (-1 == m_timerFd)
    {
        std::cout << "timer_create() failed\n";
    }
}

Timer::Handle Timer::Set(duraton_type& duration_, 
                        CallBack_type callback_)
{
    assert(!callback_.empty());
    
    timePoint_type real_time = boost::chrono::steady_clock::now() + duration_;
    m_callBacks[real_time] = std::make_pair(m_handleCounter, callback_);

    if (1 == m_callBacks.size())
    {
        std::cout << "setting timer\n";
        SetTimerIMP(duration_, callback_);
    }
    else
    {
       if (real_time == m_callBacks.begin()->first) // new timer is sooner than current
        {
            std::cout << "********** shorter time was set\n";
            m_reactor.RemFD(m_timerFd, Reactor::READ);
            struct itimerspec disarm_timeout = { {0, 0}, {0, 0}}; // disarm the timer
            if (-1 == timerfd_settime(m_timerFd, 0, &disarm_timeout, &disarm_timeout))
            {
                std::cout << "timerfd_settime() failed to disarm - X\n";
            }
            SetTimerIMP(duration_, callback_);
        }
    } 
    
    return (m_handleCounter++);
}

void Timer::SetTimerIMP(duraton_type& duration_, CallBack_type callback_)
{
    boost::chrono::nanoseconds t = boost::chrono::duration_cast<boost::chrono::nanoseconds>(duration_);
    struct timespec nsec = {0, t.count()};  // {sec, nsec}
    struct itimerspec new_timeout = { {0, 0}, nsec};     // {interval, time_value}
    
    m_reactor.AddFD(m_timerFd, Reactor::READ, 
        boost::bind(&Timer::CallBackWrapper, this, callback_));
    
    if (-1 == timerfd_settime(m_timerFd, 0, &new_timeout, NULL))
    {
        std::cout << "timerfd_settime() failed\n";
    }
    std::cout << "timer set\n";
}

void Timer::Cancel(Handle handle_)
{    
    std::map<timePoint_type, std::pair<Handle, CallBack_type> >::iterator curr = m_callBacks.begin();
    
    if (handle_ == curr->second.first) // if it is the handle of the current timer
    {
        m_reactor.RemFD(m_timerFd, Reactor::READ);
        struct itimerspec disarm_timeout = { {0, 0}, {0, 0}}; // disarm the timer
        if (-1 == timerfd_settime(m_timerFd, 0, &disarm_timeout, &disarm_timeout))
        {
            std::cout << "XXXXXXXXXX - timerfd_settime() failed to disarm\n";
        }
        m_callBacks.erase(curr++);

        duraton_type new_dur = curr->first - boost::chrono::steady_clock::now();
        SetTimerIMP(new_dur, curr->second.second);

        return;
    }
    
    while (++curr != m_callBacks.end())
    {
        if (handle_ == curr->second.first)
        {
            m_callBacks.erase(curr);
        }
    }

}

void Timer::CallBackWrapper(CallBack_type cb_)
{
    cb_();
    std::map<timePoint_type, std::pair<Handle, CallBack_type> >::iterator curr = m_callBacks.begin();
    m_callBacks.erase(curr++);
    m_reactor.RemFD(m_timerFd, Reactor::READ);

    if (!m_callBacks.empty())
    {
        duraton_type next_dur = curr->first - boost::chrono::steady_clock::now();
        SetTimerIMP(next_dur, curr->second.second);
    }
}

} // ilrd