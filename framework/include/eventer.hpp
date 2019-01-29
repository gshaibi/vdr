#ifndef EVENTER
#define EVENTER

#include <boost/noncopyable.hpp> 

#include "routines.hpp"

namespace ilrd
{

class Eventer : private boost::noncopyable
{
public:
  //constructors & destructors//
  explicit Eventer(); 
  ~Eventer() noexcept; //TODO: need dtor?

  //methods//

private:

};//class Eventer
/******************************************************************************/

} //namespae ilrd

#endif // EVENTER

