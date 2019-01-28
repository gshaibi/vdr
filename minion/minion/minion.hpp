
#ifndef MINION
#define MINION

#include <boost/core/noncopyable.hpp> // minion is non copyable
#include <boost/shared_ptr.hpp> // vdata will be managed with shared ptr

#include <vector> // data will be stored in vector<char>

#include "protocols.hpp" // protocols
#include "master_proxy.hpp"// will use master proxy for communication

namespace minion
{ 

class Minion: boost::noncopyable
{

public:

    explicit Minion(size_t numBlocks);
    ~Minion();

    void SetMasterProxy(const MasterProxy* mp_);

    void Read(const ilrd::protocols::ID& id_, size_t numBlock);
    void Write(const ilrd::protocols::ID& id_, size_t numBlock, boost::shared_ptr< std::vector<char> > data_);

private:

    typedef std::vector<char> vector_type;
    typedef boost::shared_ptr< vector_type > dataPtr_type; 

    static const size_t BLOCK_SIZE = 4096;

    MasterProxy* m_masterProxy;
    char *m_allocMemory;
};


} // namespace minion

#endif // MINION
