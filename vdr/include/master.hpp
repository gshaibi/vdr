#ifndef MASTER_HPP
#define MASTER_HPP

#include <boost/noncopyable.hpp> //boost::noncopyable
#include <boost/shared_ptr.hpp>  //boost::shared_ptr

#include <vector> //std::vector

#include "protocols.hpp"    //master-minion & master-OSP message protocols
#include "os_proxy.hpp"     //class OsProxy
#include "block_table.hpp"  //class BlockTable
#include "minion_proxy.hpp" //clsas MinionProxy

// TODO: exception safety

namespace ilrd
{

class Master : boost::noncopyable
{
public:
	Master(size_t numMinions_, Reactor& r_); 
	// NOTE: Object is incomplete before calling SetOsProxy.
	// using generated dtor. Blocked cctor & op=

	void SetOsProxy(OsProxy *osp_);

	void Read(protocols::os::ReadRequest request_);
	void Write(protocols::os::WriteRequest request_);

private:
	//data//
	OsProxy *m_osPtr;
	MinionProxy m_minionProxy;
	BlockTable m_blockTable;

	static const size_t BLOCK_SIZE = 4096;
	//friend class//
	friend class MinionProxy; //for using ReplyReadIMP & ReplyWriteIMP

	//private methods//
	void ReplyReadIMP(protocols::minion::ReadReply reply_);
	void ReplyWriteIMP(protocols::minion::WriteReply reply_);
};//class Master

} // namespace ilrd

#endif // master_hpp
