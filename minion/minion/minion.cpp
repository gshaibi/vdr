#include "minion.hpp" // header file

namespace minion
{

//****************************** SET MASTER PROXY ******************************

void Minion::SetMasterProxy(const MasterProxy* mp_)
{
    m_masterProxy = mp_;
}

//****************************** SET MASTER PROXY ******************************

// Minion(size_t numBlocks);
//     ~Minion();
//     void SetMasterProxy(MasterProxy& mp_);

//     void Read(const ID& id_, size_t numBlock);
//     void Write(const ID& id_, size_t numBlock, boost::shared_ptr< vector<char> > data_);

// private:

//     typedef vector<char> vector_type;
//     typedef boost::shared_ptr< vector_type > dataPtr_type; 

//     static const size_t BLOCK_SIZE = 4096;

//     MasterProxy& m_masterProxy;
//     char *m_allocMemory;

} // namespace minion
