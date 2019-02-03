#include <iostream>
#include <cassert>
#include <boost/bind.hpp>   // to wrap the callback
#include <sys/timerfd.h>    // timerfd
#include <stdio.h>
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

    // if our map was empty until now - we must set the tier and Add() to the reactor
    if (1 == m_callBacks.size())
    {
        m_reactor.AddFD(*m_timerFd, Reactor::READ, 
            boost::bind(&Timer::CallBackWrapper, this));
        SetTimerIMP(duration_);
    }
    else
    {  // if the new timer request is sooner than current timer - override current
       if (real_time == m_callBacks.begin()->first) 
        {
            ilrd::Log("Timer::Set() shorter time was set");
            SetTimerIMP(duration_);
        }
    } 
    
    return (m_handleCounter++);
}

void Timer::SetTimerIMP(Duration duration_)
{
    ilrd::Log("Timer::SetTimerIMP()");

    struct timespec nsec = {duration_.count() / NANOS_IN_SECOND, // seconds
                            duration_.count() % NANOS_IN_SECOND};// nanoseconds
    struct itimerspec new_timeout = { {0, 0}, nsec};    // {interval, time_value}

    if (-1 == timerfd_settime(*m_timerFd, 0, &new_timeout, NULL))
    {
        ilrd::Log("Timer::SetTimerIMP timerfd_settime() failed");
        perror("Timer::SetTimerIMP");
        //TODO: throw runtime_error(std::perror)
    }
}

void Timer::Cancel(Handle handle_)
{    
    ilrd::Log("Timer::Cancel()");
    assert(!m_callBacks.empty());
    
    std::map<TimePoint, std::pair<Handle, CallBack> >::iterator curr = m_callBacks.begin();
    
    // if it is the handle of the current timer
    if (handle_ == curr->second.first) 
    {
        m_callBacks.erase(curr++);

        // if empty - cancel timer and remove fd frome reactor
        if (m_callBacks.empty())
        {
            ilrd::Log("cancel last");
            SetTimerIMP(boost::chrono::nanoseconds(0));
            m_reactor.RemFD(*m_timerFd, Reactor::READ);
            return;
        }

        // set new timer with the duration of the current map.begin()
        Duration new_dur = curr->first - boost::chrono::steady_clock::now();
        SetTimerIMP(new_dur);
        return;
    }
    
    // if it was not the handle to the current timer - 
    // just iterate and remove the handle from the map
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
    std::map<TimePoint, std::pair<Handle, CallBack> >::iterator curr = m_callBacks.begin();

    curr->second.second(); // callback()
    m_callBacks.erase(curr++);

    // if empty - cancel timer and remove fd frome reactor
    if (m_callBacks.empty())
    {
        SetTimerIMP(boost::chrono::nanoseconds(0));
        m_reactor.RemFD(*m_timerFd, Reactor::READ);
    }
    else
    {
        Duration next_dur = curr->first - boost::chrono::steady_clock::now();

        // if we are already late to run the next callback - set timer to 1 nanosec
        if (0 > next_dur.count())
        {
            SetTimerIMP(boost::chrono::nanoseconds(1));
        }
        else
        {
            SetTimerIMP(next_dur);
        }
        
    }
}

void Timer::CloseFD(int* fd_)
{
    close(*fd_);
    delete fd_;
    fd_ = NULL;
}

} // ilrd