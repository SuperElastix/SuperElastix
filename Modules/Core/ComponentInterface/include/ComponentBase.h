#ifndef ComponentBase_h
#define ComponentBase_h

#include <iostream>
#include <cstring>
namespace elx
{
  enum interfaceStatus { success, noaccepter, noprovider };

  class ComponentBase {
  public:
    virtual interfaceStatus ConnectFrom(const char *, ComponentBase*) = 0;
    virtual int ConnectFrom(ComponentBase*) = 0;
  //protected:
    virtual ~ComponentBase() {};
  };
} // end namespace elx
#endif // #define ComponentBase_h