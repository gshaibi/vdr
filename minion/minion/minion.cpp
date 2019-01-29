#include <iostream> // prints
#include <cstring> // memcpy

#include "minion.hpp" // header file

namespace minion
{

//****************************** MINION CTOR ***********************************

Minion::Minion(size_t numBlocks): m_allocMemory(new char[numBlocks * BLOCK_SIZE]), 
                                  m_masterProxy(NULL) 
{   
    std::cout << "minion:: Ctor()" << std::endl;
}

//****************************** MINION DTOR ***********************************

Minion::~Minion()
{   
    delete[] m_allocMemory;
    std::cout << "minion:: Dtor()" << std::endl;
}

//****************************** SET MASTER PROXY ******************************

void Minion::SetMasterProxy(MasterProxy* mp_)
{
    std::cout << "minion:: SetMasterProxy(). MP address = " << mp_ << std::endl;
    m_masterProxy = mp_;
}

//******************************** IS ACTION SUCCEED ***************************

int Minion::IsActionSucceed(dataPtr_type data, int numBlock)
{
    std::cout << "minion:: IsActionSucceed()" << std::endl;
    return (0 == memcmp(&(*data)[0], m_allocMemory + (numBlock * BLOCK_SIZE),
                                                                 BLOCK_SIZE));
}

//************************************ READ ************************************

void Minion::Read(ID_type& id_, size_t numBlock)
{
    std::cout << "minion:: Read(). numBlock = " << numBlock << std::endl;   

    // create new buufer managed by shared ptr 
    dataPtr_type data(new container_type(BLOCK_SIZE)); // TODO what if throws

    // copy data
    memcpy(&(*data)[0], m_allocMemory + (numBlock * BLOCK_SIZE), BLOCK_SIZE);

    // take status
    int status = IsActionSucceed(data, numBlock);

    // send reply
    m_masterProxy->ReplyRead(id_, status, *data);
}

//************************************ WRITE ***********************************

void Minion::Write(ID_type& id_, size_t numBlock, dataPtr_type data_)
{
    std::cout << "minion:: Write(). numBlock = " << numBlock << std::endl;   

    // copy data
    memcpy(m_allocMemory + (numBlock * BLOCK_SIZE), &(*data_)[0],  BLOCK_SIZE);

    // take status
    int status = IsActionSucceed(data_, numBlock);

    // send reply
    m_masterProxy->ReplyWrite(id_, status);
}

} // namespace minion
