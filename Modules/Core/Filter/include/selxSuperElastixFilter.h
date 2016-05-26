#ifndef selxSuperElastixFilter_h
#define selxSuperElastixFilter_h

#include "itkImageSource.h"
#include "itkImageToImageFilter.h"
#include "itkMesh.h"

#include <itkConnectedRegionsMeshFilter.h>
#include <itkAbsImageFilter.h>

#include "selxBlueprint.h"
#include "Overlord.h"
/**
 * \class SuperElastixFilter
 * \brief ITK Filter interface to the SuperElastix registration library.
 */

namespace selx
{

template< typename ComponentTypeList >
class SuperElastixFilter : public itk::ProcessObject
{
public:

  /** Standard ITK typedefs. */
  typedef SuperElastixFilter                   Self;
  typedef itk::ProcessObject Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro(Self, itk::ProcessObject);
  typedef itk::DataObject InputDataType;
  typedef itk::DataObject OutputDataType;
  typedef itk::DataObject DataObject;

  typedef itk::Image<bool,1> DummyDataObjectType;

  //TODO make const correct
  //itkSetConstObjectMacro(Blueprint,Blueprint)
  itkSetObjectMacro(Blueprint, Blueprint)
  
  /** SetInput accepts any input data as long as it is derived from itk::DataObject */
  void SetInput(const DataObjectIdentifierType&, InputDataType*) ITK_OVERRIDE;
  
  /** Non type-specific GetOutput */
  OutputDataType * GetOutput(const DataObjectIdentifierType&) ITK_OVERRIDE;

  /** GetOutput tries dynamic cast to required output type */
  template<typename ReturnType>
  ReturnType* GetOutput(const DataObjectIdentifierType&);

protected:

  SuperElastixFilter(void);

  virtual void GenerateOutputInformation(void) ITK_OVERRIDE;
  virtual void GenerateData(void) ITK_OVERRIDE;


private:
  //TODO make const correct
  //Blueprint::ConstPointer m_Blueprint;
  Blueprint::Pointer m_Blueprint;
  Overlord::Pointer m_Overlord;

  typedef itk::Image<float, 2> ImageType;
  typedef itk::AbsImageFilter< ImageType, ImageType > ImageFilterType;

  typedef itk::Mesh<float, 2> MeshType;
  typedef itk::ConnectedRegionsMeshFilter<MeshType, MeshType> MeshFilterType;
  ImageFilterType::Pointer m_imageFilter;
  MeshFilterType::Pointer m_meshFilter;
};

} // namespace elx

#ifndef ITK_MANUAL_INSTANTIATION
#include "selxSuperElastixFilter.hxx"
#endif

#endif // selxSuperElastixFilter_h
