/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef selxItkToNiftiImageComponent_h
#define selxItkToNiftiImageComponent_h

#include "selxSuperElastixComponent.h"
#include "selxInterfaces.h"
#include "selxNiftyregInterfaces.h"
#include "selxItkToNiftiImage.h"
#include "selxItkObjectInterfaces.h"

#include <string.h>

#include "selxAnyFileReader.h"
#include "selxFileReaderDecorator.h"

namespace selx
{
template<  int Dimensionality, class TPixel >
class ItkToNiftiImageComponent :
        public SuperElastixComponent<
                Accepting< itkImageInterface< Dimensionality, TPixel >, itkImageMaskInterface< Dimensionality, TPixel > >,
                Providing< NiftyregReferenceImageInterface< TPixel >, NiftyregFloatingImageInterface< TPixel >,
                        NiftyregWarpedImageInterface< TPixel >,
                        NiftyregInputMaskInterface< TPixel >, NiftyregInputFloatingMaskInterface< TPixel > > >
{
public:

  /** Standard ITK typedefs. */
  typedef ItkToNiftiImageComponent< Dimensionality, TPixel > Self;
  typedef SuperElastixComponent<
          Accepting< itkImageInterface< Dimensionality, TPixel >, itkImageMaskInterface< Dimensionality, TPixel > >,
          Providing< NiftyregReferenceImageInterface< TPixel >, NiftyregFloatingImageInterface< TPixel >,
                  NiftyregWarpedImageInterface< TPixel >,
                  NiftyregInputMaskInterface< TPixel >, NiftyregInputFloatingMaskInterface< TPixel > >
  > Superclass;
  typedef std::shared_ptr< Self >       Pointer;
  typedef std::shared_ptr< const Self > ConstPointer;

  ItkToNiftiImageComponent( const std::string & name, LoggerImpl & logger );
  ~ItkToNiftiImageComponent();

  using ItkImageType = itk::Image< TPixel, Dimensionality >;
  using ItkImagePointer = typename ItkImageType::Pointer;
  using ImportFilterType = itk::ImportImageFilter< TPixel, Dimensionality >;
  using ImportFilterPointer = typename itk::ImportImageFilter< TPixel, Dimensionality >::Pointer;

  // accepting interfaces
  int Accept( typename itkImageInterface< Dimensionality, TPixel >::Pointer ) override;
  int Accept( typename itkImageMaskInterface< Dimensionality, TPixel >::Pointer ) override;

  // providing interfaces
  //virtual std::shared_ptr<nifti_image> GetFloatingNiftiImage() override;
  std::shared_ptr< nifti_image > GetReferenceNiftiImage() override;
  std::shared_ptr< nifti_image > GetFloatingNiftiImage() override;
  std::shared_ptr< nifti_image > GetWarpedNiftiImage() override;
  std::shared_ptr< nifti_image > GetInputMask() override;
  std::shared_ptr< nifti_image > GetInputFloatingMask() override;

  bool MeetsCriterion( const ComponentBase::CriterionType & criterion ) override;
  bool ConnectionsSatisfied() override;

  static const char * GetDescription() { return "ItkToNiftiImage Component"; }

private:

  ItkImagePointer m_Image;

protected:

  // return the class name and the template arguments to uniquely identify this component.
  static inline const std::map< std::string, std::string > TemplateProperties()
  {
    return { { keys::NameOfClass, "ItkToNiftiImageComponent" }, { keys::PixelType, PodString< TPixel >::Get() },
             { keys::Dimensionality, std::to_string( Dimensionality ) } };
  }
};
} //end namespace selx
#ifndef ITK_MANUAL_INSTANTIATION
#include "selxItkToNiftiImageComponent.hxx"
#endif
#endif // #define selxItkToNiftiImageComponent_h
