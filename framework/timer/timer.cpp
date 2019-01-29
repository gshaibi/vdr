#include <iostream>
#include <cassert>

#include "routines.hpp"
#include "timer.hpp"

namespace ilrd
{

Timer::Timer(Reactor& r_) : m_r(r_), m_pq()
{}

Timer::Handle Timer::Set(duration_type duration_, CallBack_type callback_)
{
    if (0 == m_pq.size())   // need to set socket
    {

    }
    else if (duration_ < m_pq.top().GetTime()) // should run first
    {
        
    }
    else
    {
        m_pq.push(TimedCallBack(duration_ + boost::chrono::steady_clock::now()
            , callback_));
    }
    
    
}

void Timer::Cancel(Handle handle_)
{

}


} // ilrd