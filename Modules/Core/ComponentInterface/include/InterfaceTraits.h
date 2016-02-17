#ifndef InterfaceTraits_h
#define InterfaceTraits_h

//TODO: note, maybe this functionality shouldn't be called a Trait, since we use a method ::Get(). Is Policy a better name?

#include "Interfaces.h"
#include "itkMacro.h"
namespace selx
{
// Traits to get printable interface name
// default implementation
template <typename T>
struct InterfaceName
{
  static const char* Get()
  {
    static_assert(false, "Please implement a template specialization for the appropriate InterfaceName");
    itkExceptionMacro("Please implement a template specialization for the appropriate InterfaceName");
    return typeid(T).name();
  }
};

// a specialization for each type of those you want to support
// and don't like the string returned by typeid
template <>
struct InterfaceName < MetricValueInterface >
{
  static const char* Get()
  {
    return "MetricValueInterface";
  }
};
template <>
struct InterfaceName < MetricDerivativeInterface >
{
  static const char* Get()
  {
    return "MetricDerivativeInterface";
  }
};
template <>
struct InterfaceName < OptimizerUpdateInterface >
{
  static const char* Get()
  {
    return "OptimizerUpdateInterface";
  }
};

template <>
struct InterfaceName < ConflictinUpdateInterface >
{
  static const char* Get()
  {
    return "ConflictinUpdateInterface";
  }
};

template <>
struct InterfaceName < TransformedImageInterface >
{
  static const char* Get()
  {
    return "TransformedImageInterface";
  }
};

template <>
struct InterfaceName < itkProcessObjectInterface >
{
  static const char* Get()
  {
    return "itkProcessObjectInterface";
  }
};

template <>
struct InterfaceName < itkImageToImageFilterInterface >
{
  static const char* Get()
  {
    return "itkImageToImageFilterInterface";
  }
};

// InterfaceName<T>::Get() should return "itkImageSourceInterface" no matter over which arguments itkImageSourceInterface is templated
template <int D, class TPixel>
struct InterfaceName < itkImageSourceInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkImageSourceInterface";
  }
};

template <int D, class TPixel>
struct InterfaceName < itkImageSourceFixedInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkImageSourceFixedInterface";
  }
};
template <int D, class TPixel>
struct InterfaceName < itkImageSourceMovingInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkImageSourceMovingInterface";
  }
};

template <int D, class TPixel>
struct InterfaceName < GetItkImageInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "GetItkImageInterface";
  }
};

template <int D, class TPixel>
struct InterfaceName < DisplacementFieldItkImageSourceInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "DisplacementFieldItkImageSourceInterface";
  }
};




template <>
struct InterfaceName < SourceInterface >
{
  static const char* Get()
  {
    return "SourceInterface";
  }
};

template <>
struct InterfaceName < SinkInterface >
{
  static const char* Get()
  {
    return "SinkInterface";
  }
};

template <int D, class TPixel>
struct InterfaceName < itkMetricv4Interface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkMetricv4Interface";
  }
};

template <>
struct InterfaceName < RunRegistrationInterface >
{
  static const char* Get()
  {
    return "RunRegistrationInterface";
  }
};

template <>
struct InterfaceName < AfterRegistrationInterface >
{
  static const char* Get()
  {
    return "AfterRegistrationInterface";
  }
};

// partial specialization of InterfaceName
// InterfaceName<T>::Get() should return the same name no matter whether T is an acceptor or provider interface.
template<typename T1>
struct InterfaceName < InterfaceAcceptor<T1> > {
  static const char* Get()
  {
    return InterfaceName<T1>::Get();
  }
};

} // end namespace selx


#endif // #define InterfaceTraits_h
