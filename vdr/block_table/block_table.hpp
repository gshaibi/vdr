#ifndef BLOCK_TABLE_HPP
#define BLOCK_TABLE_HPP

#include <vector> //using std::vector

#include <boost/noncopyable.hpp> //using boost::noncopyable

namespace ilrd
{

class BlockTable : private boost::noncopyable
{
public:
    BlockTable::BlockTable(size_t blockSize_);
    std::vector<BlockLocation> BlockTable::Translate(size_t offset_) const

private:
	size_t m_blockSize;
};

} // namespace ilrd

#endif // BLOCK_TABLE_HPP