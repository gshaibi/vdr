#ifndef sockpair_hpp
#define sockpair_hpp

#include "routines.hpp" //noexcept
#include <sys/socket.h>
#include <sys/types.h>

namespace ilrd
{

class Sockpair
{
  public:
    explicit Sockpair(int domain_, int type_, int protocol_);
    ~Sockpair() noexcept;

    int GetFirst() const;
    int GetSecond() const;

  private:
    int m_sv[2];
};

} // namespace ilrd

#endif // sockpair_hpp
