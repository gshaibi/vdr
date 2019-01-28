#include <cassert>
#include <iostream>
#include <algorithm>

#include "block_table.hpp"
#include "routines.hpp"

namespace ilrd
{

BlockTable::BlockTable(size_t numBlocks_, size_t numMinions_)
    : m_numBlocks(numBlocks_), m_blockGroup2Minions()
{
    size_t numBlockGroups(numMinions_);
    size_t blocksInGroup(numBlocks_ / numBlockGroups);

    size_t mainGroupStartBlock(0);
    size_t backupGroupStartBlock(blocksInGroup);

    for (size_t blockGroup = 0; blockGroup < numBlockGroups; ++blockGroup)
    {
        MinionID mainMinion(blockGroup);
        MinionID backupMinion((mainMinion + 1) % numMinions_);

        m_blockGroup2Minions.insert(std::make_pair(blockGroup, MinionBlockGroup(mainMinion, mainGroupStartBlock)));
		m_blockGroup2Minions.insert(std::make_pair(blockGroup, MinionBlockGroup(backupMinion, backupGroupStartBlock)));
    }
}

std::vector<BlockTable::MinionBlock> BlockTable::Translate(size_t block_) const
{
	std::pair<std::unordered_multimap<Block, MinionBlockGroup>::const_iterator, std::unordered_multimap<Block, MinionBlockGroup>::const_iterator> respMinions (m_blockGroup2Minions.equal_range(block_));

	std::vector<BlockTable::MinionBlock> ret(m_blockGroup2Minions.count(block_));

	std::copy(respMinions.first, respMinions.second, ret);
	return ret;
}

} // namespace ilrd
