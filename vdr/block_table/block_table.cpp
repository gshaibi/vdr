#include <cassert>
#include <sstream>

#include "logger.hpp"
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

	//New and not working.
    Block blk = offset_ / m_blkSize;
    size_t offsetInGroup = blk % m_numBlocksPerGroup;
    BlockGroup blkGroup = blk / m_numBlocksPerGroup;

	{
		std::stringstream str;
		str << "[BlockTable] Got translate request for offset - " << offset_ << std::endl;
		str << "[BlockTable] Block - " << blk << " OffsetInGroup - " << offsetInGroup << " Block Group - " << blkGroup << std::endl;
		Log(str.str());
		
	}
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

	assert(ret.size() == 2);

	assert(ret[0].minionID == 0);
	assert(ret[0].blockOffset == blk);

	assert(ret[1].minionID == 0);
	assert(ret[1].blockOffset == blk + m_numBlocksPerGroup);

	{
		std::stringstream str;
		str << "[BlockTable] Returning block locations - \n";
		for (int i = 0; i < ret.size(); ++i)
		{
			str << "\t Minion - " << ret[i].minionID << ", Block - " << ret[i].blockOffset << std::endl;
		}
		Log(str.str());
	}

    return ret;


	//Old and working.
	// std::vector<BlockLocation> info;

    // BlockLocation blockLocation;
    // blockLocation.minionID = 0;
    // blockLocation.blockOffset = offset_ / m_blkSize;

    // info.push_back(blockLocation);

    // return info;
}

} // namespace ilrd