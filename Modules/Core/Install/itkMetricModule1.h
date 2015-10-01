#ifndef itkMetricModule1_h
#define itkMetricModule1_h

#include "itkModuleBase.h"

namespace itk
{
class MetricModule1 : public ModuleBase
{
public:
  /** Standard class typedefs. */
  typedef MetricModule1                        Self;
  typedef ModuleBase Superclass;
  typedef SmartPointer< Self >             Pointer;
  typedef SmartPointer< const Self >       ConstPointer;

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MetricModule1, ModuleBase);

  /** define the Clone method */
  // itkCloneMacro(Self);

  /** Type of the scalar representing coordinate and vector elements. */
  //typedef  TScalar ScalarType;
  
  typedef Superclass::CriteriaType CriteriaType;
  typedef Superclass::CriteriumType CriteriumType;

  std::string GetModuleTypeAsString() const;
protected:
  /**
   * Clone the current Metric.
   * This does a complete copy of the Metric
   * state to the new Metric
   */
  virtual LightObject::Pointer InternalClone() const ITK_OVERRIDE;

  MetricModule1();
  virtual ~MetricModule1()
  {
  }

private:
  MetricModule1(const Self &);      // purposely not implemented
  void operator=(const Self &); // purposely not implemented


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
  virtual void Read();
  virtual void Write();
  virtual bool MeetsCriteria(const CriteriaType&);
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMetricModule1.hxx"
#endif

#endif
