#ifndef itkMetricComponent1_h
#define itkMetricComponent1_h

#include "ComponentBase.h"
#include "Interfaces.h"

namespace selx
{
class MetricComponent1 : 
  public Implements <
  Accepting< TransformedImageInterface>,
  Providing < MetricValueInterface >
  >
{
public:
  /** Standard class typedefs. */
  typedef MetricComponent1                        Self;
  typedef ComponentBase Superclass;
  typedef itk::SmartPointer< Self >             Pointer;
  typedef itk::SmartPointer< const Self >       ConstPointer;

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MetricComponent1, ComponentBase);
  
  typedef Superclass::CriteriaType CriteriaType;
  typedef Superclass::CriteriumType CriteriumType;

  virtual int Set(TransformedImageInterface *);
  virtual int GetValue(){ return 0; }

  //std::string GetComponentTypeAsString() const;
  //static const char * GetName(){ return "MetricComponent1"; };
  static const char * GetDescription(){ return "Example Metric Component 1"; };
protected:

  MetricComponent1();
  virtual ~MetricComponent1()
  {
  }

private:
  MetricComponent1(const Self &);      // purposely not implemented
  void operator=(const Self &); // purposely not implemented

  virtual bool MeetsCriterium(const CriteriumType &criterium);
  //virtual bool MeetsCriteria(const CriteriaType&);

};
} // end namespace selx

//#ifndef ITK_MANUAL_INSTANTIATION
//#include "itkMetricComponent1.hxx"
//#endif

#endif
