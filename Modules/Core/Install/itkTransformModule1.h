#ifndef itkTransformModule1_h
#define itkTransformModule1_h

#include "itkModuleBase.h"

namespace itk
{
template <typename TScalar,
          unsigned int NInputDimensions = 3,
          unsigned int NOutputDimensions = 3>
class TransformModule1 : public ModuleBaseTemplate< TScalar >
{
public:
  /** Standard class typedefs. */
  typedef TransformModule1                        Self;
  typedef ModuleBaseTemplate< TScalar > Superclass;
  typedef SmartPointer< Self >             Pointer;
  typedef SmartPointer< const Self >       ConstPointer;

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TransformModule1, ModuleBaseTemplate);

  /** define the Clone method */
  //itkCloneMacro(Self);

  /** Type of the scalar representing coordinate and vector elements. */
  typedef  TScalar ScalarType;

  /** Dimension of the domain space. */
  itkStaticConstMacro(InputSpaceDimension, unsigned int, NInputDimensions);
  itkStaticConstMacro(OutputSpaceDimension, unsigned int, NOutputDimensions);

  /** Get the size of the input space */
  unsigned int GetInputSpaceDimension(void) const ITK_OVERRIDE
  {
    return NInputDimensions;
  }

    /** Get the size of the output space */
    unsigned int GetOutputSpaceDimension(void) const ITK_OVERRIDE
  {
    return NOutputDimensions;
  }

  /** Standard coordinate point type for this class */
  //typedef Point<TScalar, NInputDimensions>  InputPointType;
  //typedef Point<TScalar, NOutputDimensions> OutputPointType;

  //virtual OutputPointType TransformPoint(const InputPointType  &) const = 0;
  virtual std::string GetModuleTypeAsString() const;
protected:
  /**
   * Clone the current transform.
   * This does a complete copy of the transform
   * state to the new transform
   */
  virtual typename LightObject::Pointer InternalClone() const ITK_OVERRIDE;

  TransformModule1();
  virtual ~TransformModule1()
  {
  }

private:
  TransformModule1(const Self &);      // purposely not implemented
  void operator=(const Self &); // purposely not implemented

  template <typename TType>
  std::string GetModuleTypeAsString(TType *) const
  {
    std::string rval("other");

    return rval;
  }

  std::string GetModuleTypeAsString(float *) const
  {
    std::string rval("float");

    return rval;
  }

  std::string GetModuleTypeAsString(double *) const
  {
    std::string rval("double");

    return rval;
  }

};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTransformModule1.hxx"
#endif

#endif
