#include <iostream> // prints
#include <cstring> // memcpy, memcmp
#include <cassert> // asserts

#include "logger.hpp" // writing to log
#include "minion.hpp" // header file

namespace minion
{

//****************************** MINION CTOR ***********************************

Minion::Minion(size_t numBlocks): m_allocMemory(new char[numBlocks * BLOCK_SIZE]), 
                                  m_masterProxy(NULL) 
{   
    ilrd::Log("Minion:: Ctor");
}

//****************************** MINION DTOR ***********************************

Minion::~Minion()
{   
    delete[] m_allocMemory;
    ilrd::Log("Minion:: Dtor");
}

//****************************** SET MASTER PROXY ******************************

void Minion::SetMasterProxy(MasterProxy* mp_)
{
    assert(NULL == m_masterProxy); // prevent double set
    assert(NULL != mp_);
    
    ilrd::Log("Minion:: SetMasterProxy");
    m_masterProxy = mp_;
}

//******************************** IS ACTION SUCCEED ***************************

int Minion::RetriveCopyStatus(dataPtr_type data, int numBlock)
{
    int status = (0 != memcmp(&(*data)[0], m_allocMemory + (numBlock * BLOCK_SIZE),
                                                                 BLOCK_SIZE));
    std::stringstream ss;
    ss << "Minion:: RetriveCopyStatus. result = " << status << std::endl;
    ilrd::Log(ss.str());

    return status;
}

//************************************ READ ************************************

void Minion::Read(ID_type& id_, size_t numBlock)
{
    std::stringstream ss;
    ss << "Minion:: Read. id = " << id_.GetID() << " and numBlock = " << numBlock << std::endl;
    ilrd::Log(ss.str());

    // create new buufer managed by shared ptr 
    dataPtr_type data(new container_type(BLOCK_SIZE)); // TODO what if throws

    // copy data
    memcpy(&(*data)[0], m_allocMemory + (numBlock * BLOCK_SIZE), BLOCK_SIZE);

    // take status
    int status = RetriveCopyStatus(data, numBlock);

    // send reply
    m_masterProxy->ReplyRead(id_, status, data);
}

//************************************ WRITE ***********************************

void Minion::Write(ID_type& id_, size_t numBlock, dataPtr_type data_)
{
    std::stringstream ss;
    ss << "Minion:: Write. id = " << id_.GetID() << " and numBlock = " << numBlock << std::endl;
    ilrd::Log(ss.str());

    // copy data
    memcpy(m_allocMemory + (numBlock * BLOCK_SIZE), &(*data_)[0],  BLOCK_SIZE);

    // take status
    int status = RetriveCopyStatus(data_, numBlock);

    // send reply
    m_masterProxy->ReplyWrite(id_, status);
}

} // namespace minion
