#ifndef itkMetricModule1_h
#define itkMetricModule1_h

#include "itkModuleBase.h"

namespace itk
{
template <typename TScalar,
          unsigned int NInputDimensions = 3,
          unsigned int NOutputDimensions = 3>
class MetricModule1 : public ModuleBaseTemplate< TScalar >
{
public:
  /** Standard class typedefs. */
  typedef MetricModule1                        Self;
  typedef ModuleBaseTemplate< TScalar > Superclass;
  typedef SmartPointer< Self >             Pointer;
  typedef SmartPointer< const Self >       ConstPointer;

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MetricModule1, ModuleBaseTemplate);

  /** define the Clone method */
  // itkCloneMacro(Self);

  /** Type of the scalar representing coordinate and vector elements. */
  typedef  TScalar ScalarType;
  std::string GetModuleTypeAsString() const;
protected:
  /**
   * Clone the current Metric.
   * This does a complete copy of the Metric
   * state to the new Metric
   */
  virtual typename LightObject::Pointer InternalClone() const ITK_OVERRIDE;

  MetricModule1();
  virtual ~MetricModule1()
  {
  }

private:
  MetricModule1(const Self &);      // purposely not implemented
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
#include "itkMetricModule1.hxx"
#endif

#endif
