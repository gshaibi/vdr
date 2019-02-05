#ifndef blk_TABLE_HPP
#define blk_TABLE_HPP

#include <vector> //using std::vector
#include <unordered_map>

#include <boost/noncopyable.hpp> //using boost::noncopyable

namespace ilrd
{

class BlockTable : private boost::noncopyable
{
public:
	BlockTable(size_t blkSize_, size_t numblks_, size_t numMinions_);
	
	struct BlockLocation
	{
		size_t minionID;
		size_t blkOffset;
	};

	std::vector<BlockLocation> Translate(size_t offset_) const;

private:
	typedef size_t Block;
	typedef size_t BlockGroup;
	typedef size_t MinionID;

	const size_t m_blkSize;
	const size_t m_numMinions; //TODO: Maybe numBlkGroups instead?
	const size_t m_numBlocksPerGroup;

	const size_t GROUPS_PER_MINION = 2; //TODO: Config? Name?

	struct BlockGroupLocation //TODO: Maybe pair instead?
	{
		MinionID m_minion;
		Block m_begin;
	};
	
	std::unordered_multimap<BlockGroup, BlockGroupLocation> m_blkGroup2Location;
};//class BlockTable

} // namespace ilrd

#endif // blk_TABLE_HPP