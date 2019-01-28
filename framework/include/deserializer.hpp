#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <fstream>
#include <boost/noncopyable.hpp>

#include "factory.hpp"

namespace ilrd
{

template <typename T> // T is the base class of the serialized classes.
class Deserializer : private boost::noncopyable
{
public:
    void Serialize(std::ostream& os_, const T& data_) const;
    boost::shared_ptr<T> Deserialize(std::istream& is_) const;

    template <typename D> // D must inherent from T, and implement operator <<.
    void Add();

private:
    Factory<T, std::string, std::istream&> m_factory;

    template <typename D> 
	static boost::shared_ptr<D> CreateImp(std::istream& is_);
};

template <typename T>
void Deserializer<T>::Serialize(std::ostream& os_, const T& data_) const
{
    os_ << typeid(data_).name() << std::endl << data_ << std::endl;
}

template <typename T>
template <typename D> 
void Deserializer<T>::Add()
{
    m_factory.Add(typeid(D).name(), CreateImp<D>);
}

template <typename T>
template <typename D>
boost::shared_ptr<D> Deserializer<T>::CreateImp(std::istream& is_)
{
    boost::shared_ptr<D> ret(new D);
    is_ >> *ret;
    return ret;
}

template <typename T>
boost::shared_ptr<T> Deserializer<T>::Deserialize(std::istream& is_) const
{
    std::string typeName;
    is_ >> typeName;
    return m_factory.Create(typeName, is_);
}

} // namespace ilrd