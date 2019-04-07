
#ifndef selxComposeDisplacementFieldsImageFilter_h
#define selxComposeDisplacementFieldsImageFilter_h

#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkVectorInterpolateImageFunction.h"

namespace selx
{

/**
 * \class ComposeDisplacementFieldsImageFilter
 *
 * \brief Compose two displacement fields.
 *
 * \author Nick Tustison
 * \author Brian Avants
 *
 * \ingroup ITKDisplacementField
 *
 */

template <typename TInputImage, typename TOutputImage = TInputImage>
class ComposeDisplacementFieldsImageFilter
        : public itk::ComposeDisplacementFieldsImageFilter<TInputImage, TOutputImage>
{
public:
  typedef ComposeDisplacementFieldsImageFilter     Self;
  typedef itk::ComposeDisplacementFieldsImageFilter<TInputImage, TOutputImage>
          Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Extract dimension from input image. */
  using Superclass::ImageDimension;

  using typename Superclass::InputFieldType;
  using typename Superclass::OutputFieldType;

  /** Image typedef support. */
  using typename Superclass::PixelType;
  using typename Superclass::VectorType;
  using typename Superclass::RegionType;
  using typename Superclass::IndexType;

  using typename Superclass::PointType;
  using typename Superclass::SpacingType;
  using typename Superclass::OriginType;
  using typename Superclass::SizeType;
  using typename Superclass::DirectionType;

  /** Other typedef */
  using typename Superclass::RealType;
  using typename Superclass::InterpolatorType;

  /** Get the interpolator. */
  using Superclass::GetInterpolator;

  /** Set the deformation field */
  using Superclass::SetDisplacementField;

  /**
   * Get the deformation field.
   */
  using Superclass::GetDisplacementField;

  /** Set the warping field */
  using Superclass::SetWarpingField;

  /**
   * Get the warping field.
   */
  using Superclass::GetWarpingField;

  /* Set the interpolator. */
  using Superclass::SetInterpolator;

  /* Override default behaviour (do not check that images have the same
   * world coordinate systems). This is why we make this new class. The
   * superclass should have done this from the beginning */
  void VerifyInputInformation() override {};

  /* This function implements the default behaviour of
   * itk::ComposeDisplacementFieldsImageFilter::GenerateInputRequestedRegion()
   * EXCEPT we let the warping field keep its own requested region (i.e. do not copy
   * requested region from output to warping field) */
  void GenerateInputRequestedRegion() override {
    typedef itk::ImageBase< ImageDimension > ImageBaseType;
    ImageBaseType* displacementField = dynamic_cast< ImageBaseType * >( const_cast< InputFieldType * >( this->GetInput( 0 ) ) );
    if( displacementField )
    {
      typename ImageBaseType::RegionType inputRegion;
      this->CallCopyOutputRegionToInputRegion( inputRegion, this->GetOutput()->GetRequestedRegion() );
      displacementField->SetRequestedRegion(inputRegion);
    }
  }

  /* Same as the base class except we crop the region to ensure it is contained in both output and
   * warping field. Actually this is due to a bug in the Superclass. The output gets its domain from
   * the displacement field so the superclass _should_ loop over the displacement field and the output
   * (since these two domains are the same) NOT loop can be over the warping field and the output.
   */
  void ThreadedGenerateData( const RegionType & region, itk::ThreadIdType itkNotUsed( threadId ) ) override
  {
    typename OutputFieldType::Pointer output = this->GetOutput();
    typename InputFieldType::ConstPointer warpingField = this->GetWarpingField();

    auto croppedRegion = const_cast< RegionType & >( region );
    croppedRegion.Crop( warpingField->GetRequestedRegion() );

    itk::ImageRegionConstIteratorWithIndex<InputFieldType> warpingFieldIterator( warpingField, croppedRegion );
    itk::ImageRegionIterator<OutputFieldType> outputIterator( output, croppedRegion );

    PointType pointIn1;
    PointType pointIn2;
    PointType pointIn3;

    typename OutputFieldType::PixelType outDisplacement;

    for( warpingFieldIterator.GoToBegin(), outputIterator.GoToBegin(); !warpingFieldIterator.IsAtEnd();
            ++warpingFieldIterator, ++outputIterator )
    {
      warpingField->TransformIndexToPhysicalPoint( warpingFieldIterator.GetIndex(), pointIn1 );

      VectorType warpVector = warpingFieldIterator.Get();

      for( unsigned int d = 0; d < ImageDimension; d++ )
      {
        pointIn2[d] = pointIn1[d] + warpVector[d];
      }

      typename InterpolatorType::OutputType displacement( 0.0 );
      if( this->GetInterpolator()->IsInsideBuffer( pointIn2 ) )
      {
        displacement = this->GetInterpolator()->Evaluate( pointIn2 );
      }

      for( unsigned int d = 0; d < ImageDimension; d++ )
      {
        pointIn3[d] = pointIn2[d] + displacement[d];
      }

      outDisplacement = pointIn3 - pointIn1;

      outputIterator.Set( outDisplacement );
    }
  }

protected:

  /** Constructor */
  ComposeDisplacementFieldsImageFilter() {};

  /** Deconstructor */
  ~ComposeDisplacementFieldsImageFilter() {};

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(ComposeDisplacementFieldsImageFilter);

};

} // end namespace itk

#endif
