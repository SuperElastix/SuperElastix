#ifndef itkTransformComponent1_h
#define itkTransformComponent1_h

#include "Interfaces.h"
#include "ComponentBase.h"

namespace selx
{
class TransformComponent1 : 
  public Implements <
  Accepting< >,
  Providing < TransformedImageInterface >
  >
{
public:
  /** Standard class typedefs. */
  typedef TransformComponent1                        Self;
  typedef ComponentBase Superclass;
  typedef itk::SmartPointer< Self >             Pointer;
  typedef itk::SmartPointer< const Self >       ConstPointer;

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TransformComponent1, ComponentBase);

  typedef Superclass::CriteriaType CriteriaType;
  typedef Superclass::CriterionType CriterionType;

  int GetTransformedImage(){ return 0; };
  static const char * GetDescription(){ return "Example Transform Component 1"; };
  virtual bool MeetsCriterion(const CriterionType &criterion);
protected:
  TransformComponent1();
  virtual ~TransformComponent1()
  {
  }

private:
  TransformComponent1(const Self &);      // purposely not implemented
  void operator=(const Self &); // purposely not implemented

  //virtual bool MeetsCriteria(const CriteriaType&);
  
};
} // end namespace selx

//#ifndef ITK_MANUAL_INSTANTIATION
//#include "itkTransformComponent1.hxx"
//#endif

#endif
