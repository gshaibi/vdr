#include <cassert>

#include "block_table.hpp" 

namespace ilrd
{

BlockTable::BlockTable(size_t blkSize_, size_t numBlks_, size_t numMinions_)
    : m_blkSize(blkSize_), m_numMinions(numMinions_),
      m_numBlocksPerGroup(numBlks_ / m_numMinions), m_blkGroup2Location()
{
    assert(numBlks_ % numMinions_ == 0);
    size_t numBlkGroups(m_numMinions);

    for (size_t blkGroup = 0; blkGroup < numBlkGroups; ++blkGroup)
    {
        for (size_t i = 0; i < GROUPS_PER_MINION; i++)
        {
            m_blkGroup2Location.insert(std::make_pair(
                blkGroup, BlockGroupLocation{(blkGroup + i) % numMinions_,
                                             i * m_numBlocksPerGroup}));
        }
    }
}

std::vector<BlockTable::BlockLocation>
BlockTable::Translate(size_t offset_) const
{
    Block blk = offset_ / m_blkSize;
    size_t offsetInGroup = blk % m_numBlocksPerGroup;
    BlockGroup blkGroup = blk / m_numBlocksPerGroup;

    typedef std::multimap<BlockGroup, 
												BlockGroupLocation>::const_iterator MapIterator;

    std::pair<MapIterator, MapIterator> respMinions(
        m_blkGroup2Location.equal_range(blkGroup));

    std::vector<BlockTable::BlockLocation> ret;

    for (MapIterator i = respMinions.first; i != respMinions.second; ++i)
    {
        ret.push_back(BlockLocation{i->second.m_minion,
                                    i->second.m_begin + offsetInGroup});
    }

    return ret;
}

} // namespace ilrd