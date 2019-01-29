#include "block_table.hpp" //block_table declarations

#include "master.hpp" //using master

namespace ilrd
{

BlockTable::BlockTable(size_t blockSize_):
                                        m_blockSize(blockSize_)
                                                    
    
{
} 

std::vector<BlockTable::BlockLocation> BlockTable::Translate(size_t offset_) const
{								
    std::vector<BlockLocation> info;

    BlockLocation blockLocation;
    blockLocation.minionID = 0;
    blockLocation.blockOffset = offset_ / m_blockSize;

    info.push_back(blockLocation);

    return info;
}

} //namespace ilrd