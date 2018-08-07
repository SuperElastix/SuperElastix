#include <cmath>

namespace selx
{
namespace Functor
{
template< class TPixel>
class IsNan
{
public:
  IsNan() {}
  ~IsNan() {}
  bool operator!=(const IsNan &) const
  {
    return false;
  }
 
  bool operator==(const IsNan & other) const
  {
    return !( *this != other );
  }
 
  inline TPixel operator()(const TPixel & A ) const
  {
    return std::isnan( A );
  }
};
}
}