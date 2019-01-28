#ifndef protocols_hpp
#define protocols_hpp

#include <boost/shared_ptr.hpp>
#include <cstddef> //size_t
#include <vector>

#include <linux/types.h> //using __be64 __be32 in udp protocol

namespace ilrd
{

class OsProxy;

namespace protocols
{
	

class ID
{
public:
    explicit ID(char id_[8]);
    // generated cctor and dtor.

	bool operator==(const ID& other_); //TODO: compare char m_id
	bool operator<(const ID& other_); //TODO: might be needed for container use

private:
    friend class ilrd::OsProxy;
	friend class ilrd::MinionProxy;

    const char* GetID() const;

    static const size_t ID_ARR_SIZE = 8;

    char m_id[ID_ARR_SIZE];
};

namespace os
{

class ReadRequest
{
public:
    explicit ReadRequest(const ID& id_, size_t len_, size_t offset_);
    // generated dtor cctor and assignment operator.

    const ID& GetID() const;
    size_t GetLen() const;
    size_t GetOffset() const;

private:
    ID m_id;
    size_t m_len;
    size_t m_offset;
};

class WriteRequest
{
public:
    typedef boost::shared_ptr<const std::vector<char> > SharedBuffer;

    explicit WriteRequest(const ID& id_,
                          SharedBuffer data_,
                          size_t dataIdx_, size_t offset_);
    // generated dtor cctor and assignment operator.

    const ID& GetID() const;
    const char *GetData() const; //returns pointer to internal data.
    size_t GetOffset() const;
    size_t GetLength() const;

private:
    ID m_id;
    SharedBuffer m_data;
    size_t m_dataIdx;
    size_t m_offset;
};

class WriteReply
{
public:
    explicit WriteReply(const ID& id_, int status_);

    bool GetStatus() const;
    const ID& GetID() const;

private:
    ID m_id;
    bool m_status;
};

class ReadReply : private WriteReply
{
public:
    typedef boost::shared_ptr<const std::vector<char> > SharedBuffer;

    ReadReply(const ID& id_, int status_, SharedBuffer data_);

    using WriteReply::GetID;
    using WriteReply::GetStatus;
    SharedBuffer GetData() const;

private:
    SharedBuffer m_data;
};

}//namespace os



namespace minion
{

class ReadRequest: private os::ReadRequest
{
public:
	explicit ReadRequest(const os::ReadRequest& req_, size_t srcBlock_);

	using os::ReadRequest::GetID;
	size_t GetBlock() const;

private:
	size_t m_block;
};

class WriteRequest: private os::WriteRequest
{
public:
	explicit WriteRequest(const os::WriteRequest& req_, size_t destBlock_);
	
	using os::WriteRequest::GetID;
	using os::WriteRequest::GetData;
	size_t GetBlock() const;

private:
	size_t m_block;
};

class WriteReply
{
public:
    explicit WriteReply(const ID& id_, int minionId_, int status);
    //generated dtor, cctor op=

    const ID& GetID() const;
	int GetMinionID() const; 
    int GetStatus() const;

private:
    const ID m_id;
	const int m_minionID;
    int m_status;
}; 

class ReadReply :private WriteReply
{
public:
    typedef boost::shared_ptr<const std::vector<char> > SharedBuffer;

    explicit ReadReply(int minionId_, int status, const ID& id_,
							  SharedBuffer buff_);
    //generated dtor, cctor op=

    using WriteReply::GetID;
	using WriteReply::GetMinionID; 
    using WriteReply::GetStatus; 

	SharedBuffer GetData() const;

private:
	SharedBuffer m_buff;   
};

} // namespace minion

namespace minionUDP
{

//block size
static const int BLK_SIZE = 0x1000; //protocol predefined block size (4k)

//request types
enum RequestType
{
	REQ_READ = 0,
	REQ__WRITE = 1,
};

//request
struct request 
{
	__be32 reqType;	// == READ || == WRITE 
	char reqID[8];
	__be64 blockNum;
	char data[0x1000]; //4k of data
};

// Reply 
struct reply 
{
	__be32 status;		// 0 = ok, else error
	char reqID[8];		// ID you got from request
	char data[0x1000]; //4k of data
};

} //namespace minionUDP

} // protocols

} // namespace ilrd

#endif // protocols_hpp
