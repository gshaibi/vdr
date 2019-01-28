#ifndef BLOCK_TABLE_HPP
#define BLOCK_TABLE_HPP

#include <vector> //using std::vector

#include <boost/noncopyable.hpp> //using boost::noncopyable
#include <boost/shared_ptr.hpp>  //using boost::shared_ptr
#include <map>

namespace ilrd
{

class BlockTable : private boost::noncopyable
{
public:
	static const size_t BLOCK_SIZE = 0x1000;

    BlockTable(size_t numBlocks_, size_t numMinions_);

    struct MinionBlock
    {
        size_t m_minionID;
        size_t m_block;
    };
    std::vector<MinionBlock> Translate(size_t block_) const;
    // the vector from translate can be returned by val due to its limited size
    // (few minions)

private:
	typedef size_t MinionID;
	typedef size_t Block;
	typedef std::pair<MinionID, Block> MinionBlockGroup; //pair of MinionID and beginBlock in it of the group.

	size_t m_numBlocks;
	class Block2Group
	{
		size_t operator()(Block b_)
		{
			return b_ / BLOCK_SIZE;
		}
	};
	std::unordered_multimap <Block, MinionBlockGroup> m_blockGroup2Minions; //returns MinionBlockGroup by block
};

} // namespace ilrd

#endif // BLOCK_TABLE_HPP