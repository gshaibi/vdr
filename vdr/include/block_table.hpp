#ifndef blk_TABLE_HPP
#define blk_TABLE_HPP

#include <vector> //using std::vector
#include <map>

#include <boost/noncopyable.hpp> //using boost::noncopyable

namespace ilrd
{

class BlockTable : private boost::noncopyable
{
public:
	//numMinions_ should divide numBlks_ without remainder.
	BlockTable(size_t blkSize_, size_t numBlks_, size_t numMinions_);
	//generated dtor.
	
	struct BlockLocation
	{
		size_t minionID;
		size_t blockOffset;
	};

	std::vector<BlockLocation> Translate(size_t offset_) const;

private:
	typedef size_t Block;
	typedef size_t BlockGroup;
	typedef size_t MinionID;

	const size_t m_blkSize;
	const size_t m_numMinions; //TODO: Maybe numBlkGroups instead?
	const size_t m_numBlocksPerGroup;

	const static size_t GROUPS_PER_MINION = 2; //TODO: Config? Name?

	struct BlockGroupLocation //TODO: Maybe pair instead?
	{
		MinionID m_minion;
		Block m_begin;
	};
	
	std::multimap<BlockGroup, BlockGroupLocation> m_blkGroup2Location;
};//class BlockTable

} // namespace ilrd

#endif // blk_TABLE_HPP