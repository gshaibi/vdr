#include <cassert>
#include <cstring>
#include <iostream>
#include <linux/nbd.h>

#include "protocols.hpp"
#include "routines.hpp"

namespace ilrd
{

namespace protocols
{

ID::ID(char id_[8]) : m_id() { std::memcpy(m_id, id_, sizeof(m_id)); }

const char* ID::GetID() const { return m_id; }


namespace os
{

ReadRequest::ReadRequest(const ID& id_, size_t len_, size_t offset_)
    : m_id(id_), m_len(len_), m_offset(offset_)
{
}

const ID& ReadRequest::GetID() const { return m_id; }

size_t ReadRequest::GetLen() const { return m_len; }

size_t ReadRequest::GetOffset() const { return m_offset; }

WriteRequest::WriteRequest(const ID& id_, SharedBuffer data_, size_t dataIdx_,
                           size_t offset_)
    : m_id(id_), m_data(data_), m_dataIdx(dataIdx_), m_offset(offset_)
{
}

const ID& WriteRequest::GetID() const { return m_id; }

const char* WriteRequest::GetData() const
{
    return m_data.get()->data() + sizeof(nbd_request);
}

size_t WriteRequest::GetOffset() const { return m_offset; }

size_t WriteRequest::GetLength() const
{
	return m_data->size() - m_dataIdx;
}

WriteReply::WriteReply(const ID& id_, int status_)
    : m_id(id_), m_status(status_)
{
}

bool WriteReply::GetStatus() const { return m_status; }

const ID& WriteReply::GetID() const { return m_id; }

ReadReply::ReadReply(const ID& id_, int status_, SharedBuffer data_)
    : WriteReply(id_, status_), m_data(data_)
{
}

ReadReply::SharedBuffer ReadReply::GetData() const { return m_data; }

} // os

namespace minion
{

ReadRequest::ReadRequest(const os::ReadRequest& req_, size_t srcBlock_) : os::ReadRequest(req_), m_block(srcBlock_)
{
}

size_t ReadRequest::GetBlock() const
{
	return m_block;
}

WriteRequest::WriteRequest(const os::WriteRequest& req_, size_t destBlock_) : os::WriteRequest(req_), m_block(destBlock_)
{
}

size_t WriteRequest::GetBlock() const
{
	return m_block;
}

WriteReply::WriteReply(const ID& id_, int minionId_, int status_) : m_id(id_) , m_minionID(minionId_), m_status(status_) 
{
}

const ID& WriteReply::GetID() const
{
	return m_id;
}

int WriteReply::GetMinionID() const
{
	return m_minionID;
}

int WriteReply::GetStatus() const
{
	return m_status;
}


ReadReply::ReadReply(const ID& id_, int minionId_, int status_, 
							  SharedBuffer buff_) : WriteReply(id_, minionId_, status_), m_buff(buff_)
							  {
							  }

ReadReply::SharedBuffer ReadReply::GetData() const 
{
	return m_buff;
}

} // minion




} // namespace protocol

} // namespace ilrd
