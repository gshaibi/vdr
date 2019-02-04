#ifndef BLOCK_TABLE_HPP
#define BLOCK_TABLE_HPP

#include <vector> //using std::vector
#include <unordered_map>

#include <boost/noncopyable.hpp> //using boost::noncopyable

namespace ilrd
{

class BlockTable : private boost::noncopyable
{
public:
	BlockTable(size_t blockSize_, size_t numBlocks_ = 0, size_t numMinions_ = 0);
	
	struct BlockLocation
	{
		size_t minionID;
		size_t blockOffset;
	};

	std::vector<BlockLocation> Translate(size_t offset_) const;

private:
	typedef size_t BlockGroup;
	typedef size_t MinionID;
	typedef size_t Block;

	size_t m_blockSize;

	struct BlockGroupLocation
	{
		MinionID m_minion;
		Block m_begin;
	};
	
	std::unordered_multimap<BlockGroup, BlockGroupLocation> m_blockGroup2Location;
};//class BlockTable

} // namespace ilrd

#endif // BLOCK_TABLE_HPP