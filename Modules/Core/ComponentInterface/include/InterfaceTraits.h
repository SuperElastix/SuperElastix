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

/*
auto name = InterfaceName < itkImageToImageFilterInterface >::Get()
auto name = InterfaceName < itkImageSourceInterface<3,float> >::Get()

auto name = InterfaceName < wrap<itkImageToImageFilterInterface> >::Get()

auto name = InterfaceName < wrap<itkImageSourceInterface<3, float>> >::Get()
*/

//typedef itkImageSourceInterface <D, T>
//itkImageSourceInterface<Dimensionality, PixelType>
//template <itkImageSourceInterface IF, int D, class P>
//template <int D, typename T>
//template <template <int Dname, class Pname> class IF = InterfaceName<int,class>, int D, class TPixel>
//template <template <int, class> class itkImageSourceInterface, int D, class TPixel>
//template <template <typename...> class IF, typename... Rest>
//struct InterfaceName < IF <Rest...> >
/*
template <template <int, class> class IF, int D, class TPixel>
struct InterfaceName < IF <D, TPixel> >
{
  static const char* Get()
  {
    return "itkImageSourceInterface";
  }
};
*/

template <int D, class TPixel>
struct InterfaceName < itkImageSourceInterface <D, TPixel> >
{
  static const char* Get()
  {
    return "itkImageSourceInterface";
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

/*
// partial specialization of InterfaceName
template<template<typename> class TT, typename T1>
struct InterfaceName < TT<T1> > {
  static const char* Get()
  {
    return InterfaceName<T1>::Get();
  }
};
*/

// partial specialization of InterfaceName

template<typename T1>
struct InterfaceName < InterfaceAcceptor<T1> > {
  static const char* Get()
  {
    return InterfaceName<T1>::Get();
  }
};


template <typename T>
struct AcceptorInterfaceName
{
  static const char* Get()
  {
    return InterfaceName<T>::Get();
  }
};

} // end namespace selx


#endif // #define InterfaceTraits_h
