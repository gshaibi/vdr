#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <cassert>

#include <algorithm> //std::for_each
#include <map>  //Dispatcher::m_callbacks

namespace ilrd
{

template <typename S, typename P>
class CallbackBase;

// S - subject - should Implement Register and Unregister,
//	that call the Dispatcher Register and Unregister. Unregister should be noexcept.
// P - type of the paremeter to dispatch. should be copyable.
template <typename S, typename P>
class Dispatcher : private boost::noncopyable
{
public:
    // generated ctor
    ~Dispatcher() noexcept;

    void Register(CallbackBase<S, P>* callback_);
    void Unregister(CallbackBase<S, P>* key_);

    void Dispatch(P message_);

private:
    static void SendDisconnectImp(std::pair<CallbackBase<S, P>*, bool> callbackPair_);

    std::map<CallbackBase<S, P>*, bool> m_callbacks;
};

// S, P - same as Dispatcher.
template <typename S, typename P>
class CallbackBase : private boost::noncopyable
{
public:
    explicit CallbackBase(S& subject_);
    virtual ~CallbackBase() noexcept;

protected:
    virtual void Disconnect();

private:
    friend class Dispatcher<S, P>; // for Dispatcher::Dispatch

    virtual void Do(P message_) = 0;

    S& m_subject;
    bool m_shouldUnreg;
};

// S, P - same as Dispatcher.
template <typename S, typename P, typename O>
class Callback : public CallbackBase<S, P>
{
public:
    explicit Callback(S& subject_, O& observer_, void (O::*onDispatch_)(P),
                      void (O::*onDisconnect_)() = NULL); 
					  //O::onDisconnect should be noexcept (called inside dtor).
    // generated dtor

private:
    virtual void Do(P message_);
    virtual void Disconnect();

    O& m_observer;
    void (O::*const m_onDispatch)(P);
    void (O::*const m_onDisconnect)();
};

//--------------------------------------------------Dispatcher--------------------------------------------
template <typename S, typename P>
Dispatcher<S, P>::~Dispatcher() noexcept
{
    for_each(m_callbacks.begin(), m_callbacks.end(), SendDisconnectImp);
}

template <typename S, typename P>
void Dispatcher<S, P>::Register(CallbackBase<S, P>* callback_)
{
	assert(NULL != callback_);
    m_callbacks[callback_] = true;
}

template <typename S, typename P>
void Dispatcher<S, P>::Unregister(CallbackBase<S, P>* callback_)
{
	assert(m_callbacks.end() != m_callbacks.find(callback_));
    // m_callbacks.erase(callback_); //TODO: Mark for removal. Dont remove while may be in Dispatch.
	m_callbacks[callback_] = false;
}

template <typename S, typename P>
void Dispatcher<S, P>::Dispatch(P message_)
{
    // for_each(m_callbacks.begin(), m_callbacks.end(),
    //  boost::bind(&CallbackBase<S, P>::Do, _1, message_)); 
	//TODO: May Do perform Unregister?
    for (typename std::map<CallbackBase<S, P>*, bool>::iterator i(m_callbacks.begin());
         	i != m_callbacks.end(); )
    {
		if (i->second)
		{
			(*(i++)).first->Do(message_);
		}
		else
		{
        	m_callbacks.erase(i++);
		}
    }
}

template <typename S, typename P>
void Dispatcher<S, P>::SendDisconnectImp(std::pair<CallbackBase<S, P>*, bool> callbackPair_)
{
	if (callbackPair_.second)
	{
    	callbackPair_.first->Disconnect();
	}
}

//----------------------------------CallbackBase--------------------------------------------
template <typename S, typename P>
CallbackBase<S, P>::CallbackBase(S& subject_)
    : m_subject(subject_), m_shouldUnreg(true)
{
    m_subject.Register(this);
}

template <typename S, typename P>
CallbackBase<S, P>::~CallbackBase() noexcept
{
    if (m_shouldUnreg)
    {
        m_subject.Unregister(this);
    }
}

template <typename S, typename P>
void CallbackBase<S, P>::Disconnect()
{
	assert(true == m_shouldUnreg);
    m_shouldUnreg = false;
}

//----------------------------------Callback--------------------------------------------
template <typename S, typename P, typename O>
Callback<S, P, O>::Callback(S& subject_, O& observer_,
                            void (O::*onDispatch_)(P),
                            void (O::*onDisconnect_)())
    : CallbackBase<S, P>(subject_), m_observer(observer_),
      m_onDispatch(onDispatch_), m_onDisconnect(onDisconnect_)
{
}

template <typename S, typename P, typename O>
void Callback<S, P, O>::Do(P message_)
{
    (m_observer.*m_onDispatch)(message_);
}

template <typename S, typename P, typename O>
void Callback<S, P, O>::Disconnect()
{
    CallbackBase<S, P>::Disconnect();
    if (NULL != m_onDisconnect)
    {
        (m_observer.*m_onDisconnect)();
    }
}

} // namespace ilrd

#endif // DISPATCHER_HPP