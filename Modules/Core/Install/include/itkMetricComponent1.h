#ifndef itkMetricComponent1_h
#define itkMetricComponent1_h

#include "itkComponentBase.h"

namespace itk
{
class MetricComponent1 : public ComponentBase
{
public:
  /** Standard class typedefs. */
  typedef MetricComponent1                        Self;
  typedef ComponentBase Superclass;
  typedef SmartPointer< Self >             Pointer;
  typedef SmartPointer< const Self >       ConstPointer;

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MetricComponent1, ComponentBase);

  /** define the Clone method */
  // itkCloneMacro(Self);

  /** Type of the scalar representing coordinate and vector elements. */
  //typedef  TScalar ScalarType;
  
  typedef Superclass::CriteriaType CriteriaType;
  typedef Superclass::CriteriumType CriteriumType;

  std::string GetComponentTypeAsString() const;
protected:
  /**
   * Clone the current Metric.
   * This does a complete copy of the Metric
   * state to the new Metric
   */
  virtual LightObject::Pointer InternalClone() const ITK_OVERRIDE;

  MetricComponent1();
  virtual ~MetricComponent1()
  {
  }

private:
  MetricComponent1(const Self &);      // purposely not implemented
  void operator=(const Self &); // purposely not implemented


  std::string GetComponentTypeAsString(float *) const
  {
    std::string rval("float");

    return rval;
  }

  std::string GetComponentTypeAsString(double *) const
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
#include "itkMetricComponent1.hxx"
#endif

#endif
