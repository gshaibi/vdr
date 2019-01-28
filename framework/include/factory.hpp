#ifndef factory_hpp
#define factory_hpp

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <exception>
#include <boost/noncopyable.hpp>

namespace ilrd
{

class BadFactorKey : public std::runtime_error
{
public:
	BadFactorKey() : std::runtime_error("") {}
};

template <typename T, typename K, typename P>
//TODO: Requirements for template arguments.
//T - factory product.
//K - key. must have cctor.
//P - type of parameter for the creator function. must have cctor.
class Factory : private boost::noncopyable
{
public:
	bool Add(const K& key_, boost::function<boost::shared_ptr<T> (P)> creator_); //returns whether the key was not taken.
	boost::shared_ptr<T> Create(const K& key_, P param_) const;

private:
	std::map<K ,boost::function<boost::shared_ptr<T> (P)> > m_creators;
};

template <typename T, typename K, typename P>
bool Factory<T,K,P>::Add(const K& key_, boost::function<boost::shared_ptr<T> (P)> creator_)
{
	if ( m_creators.insert(make_pair(key_, creator_)).second)
	{
		return true;
	}
	m_creators[key_] = creator_;
	return false;
}

template <typename T, typename K, typename P>
boost::shared_ptr<T> Factory<T,K,P>::Create(const K& key_, P param_) const
{
	try
	{
		return m_creators.at(key_)(param_);
	}
	catch (const std::out_of_range &err)
	{
		throw BadFactorKey();
	}
	catch(...)
	{
		throw;
	}
}

}	
#endif // factory_hpp
