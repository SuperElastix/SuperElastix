#include <iostream>
#include <string>
#include <sstream>
#include <functional>

#include <itkVersion.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkCastImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkMeanSquaresImageToImageMetric.h>
#include <itkRegularStepGradientDescentOptimizer.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkRecursiveMultiResolutionPyramidImageFilter.h>
#include <itkTransformFileWriter.h>
#include <itkMinimumMaximumImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>

#include "selxSuperElastixComponent.h"
#include "selxSinksAndSourcesInterfaces.h"
#include "selxItkObjectInterfaces.h"


// Fundamentally this is to group together all the typedefs
// The class does not truly have any state.
template<class PixelType, unsigned int Dimension>
struct PreliminaryAffineStrategy
{
    // typedef all the things
    using Coord=double;
    using Image=itk::Image<PixelType,Dimension>;
    using ImagePtr=typename Image::Pointer;
    using Point=typename Image::PointType;
    using Vector=typename Image::PointType::VectorType;
    using Size=typename Image::SizeType;
    using Index=typename Image::IndexType;
    using Region=typename Image::RegionType;
    using ContIndex=itk::ContinuousIndex<Coord,Dimension>;
    using Reader=itk::ImageFileReader<Image>;
    using ReaderPtr=typename Reader::Pointer;
    using Writer=itk::ImageFileWriter<Image>;
    using WriterPtr=typename Writer::Pointer;
    using InternalPixelType=float;
    using InternalImage=itk::Image<InternalPixelType,Dimension>;
    using InternalImagePtr=typename InternalImage::Pointer;
    using Caster=itk::CastImageFilter<Image,InternalImage>;
    using Pyramid=itk::RecursiveMultiResolutionPyramidImageFilter<
                                    Image,
                                    InternalImage >;
    using PyramidPtr=typename Pyramid::Pointer;
    using MeanSquaresMetric=itk::MeanSquaresImageToImageMetric<InternalImage,InternalImage>;
    using MeanSquaresMetricPtr=typename MeanSquaresMetric::Pointer;
    using LinearInterpolator=itk::LinearInterpolateImageFunction<Image,Coord>;
    using LinearInterpolatorPtr=typename LinearInterpolator::Pointer;
    using LinearInternalInterpolator=itk::LinearInterpolateImageFunction<InternalImage,Coord>;
    using LinearInternalInterpolatorPtr=typename LinearInternalInterpolator::Pointer;
    using AffineTransform=itk::AffineTransform<Coord,Dimension>;
    using AffineTransformPtr=typename AffineTransform::Pointer;
    using Parameters=typename AffineTransform::ParametersType;
    using RSGD=itk::RegularStepGradientDescentOptimizer;
    using RSGDPtr=RSGD::Pointer;
    using TransformWriter=itk::TransformFileWriter;
    using TransformWriterPtr=typename TransformWriter::Pointer;
    using Resampler=itk::ResampleImageFilter<Image,Image>;
    using ResamplerPtr=typename Resampler::Pointer;
    using DisplacementField = typename itk::Image< itk::Vector<float,Dimension>, Dimension>;
    using DisplacementFieldPtr =  typename DisplacementField::Pointer;
 

    
    // Params
    std::string m_strategy;
    std::string m_outputpath;
    std::function<void(const int, const std::string &)> m_log;
    
    void ProcessImages(ImagePtr iFixedImage, ImagePtr iMovingImage, DisplacementFieldPtr oDisplacementField)
    {
        
        {
            std::ostringstream oss;
            oss << "FixedImage size " << iFixedImage->GetLargestPossibleRegion().GetSize();
            m_log(2,oss.str());
        }
        {
            std::ostringstream oss;
            oss << "MovingImage size " << iMovingImage->GetLargestPossibleRegion().GetSize();
            m_log(2,oss.str());
        }
        
        // Find a center
        Parameters centre(3);
        Parameters centreMoving(3);
        {
            Point ctr;
            ContIndex index;
            const Size sz=iFixedImage->GetLargestPossibleRegion().GetSize();
            for ( unsigned int i=0;i<Dimension;++i ) { index[i]=static_cast<double>(sz[i] )/2.0; }
            iFixedImage->TransformContinuousIndexToPhysicalPoint( index,ctr );
            for ( unsigned int i=0;i<Dimension;++i ) { centre[i]=ctr[i]; }
            
        }
        {
            Point ctr;
            ContIndex index;
            const Size sz=iMovingImage->GetLargestPossibleRegion().GetSize();
            for ( unsigned int i=0;i<Dimension;++i ) { index[i]=static_cast<double>(sz[i] )/2.0; }
            iMovingImage->TransformContinuousIndexToPhysicalPoint( index,ctr );
            for ( unsigned int i=0;i<Dimension;++i ) { centreMoving[i]=ctr[i]; }
        }
        {
            std::ostringstream oss;
            oss << "Fixedimage center " << centre;
            m_log(2,oss.str());
        }
        {
            std::ostringstream oss;
            oss << "MovingImage center " << centreMoving;
            m_log(2,oss.str());
        }

        // Find number of levels
        unsigned int numberOfLevels=0;
        {
           unsigned int numberOfPixels=1;
           const Size sz=iFixedImage->GetLargestPossibleRegion().GetSize();
           for ( int i=0;i<Dimension;++i) { numberOfPixels*=sz[i]; }
           double temp=log(( double )numberOfPixels ) /log( 2.0 )/Dimension-( 7-Dimension );
           numberOfLevels=( temp<1 )?1:( unsigned int )floor( temp );
        }
        PyramidPtr fixedPyramid=Pyramid::New();
        PyramidPtr movingPyramid=Pyramid::New();
 
        // Setup the fixed image pyramid
        fixedPyramid->SetNumberOfLevels( numberOfLevels );
        fixedPyramid->SetInput( iFixedImage );
        fixedPyramid->UpdateLargestPossibleRegion();

        // Setup the moving image pyramid
        movingPyramid->SetNumberOfLevels( numberOfLevels );
        movingPyramid->SetInput( iMovingImage );
        movingPyramid->UpdateLargestPossibleRegion();
        
        AffineTransformPtr affineTransform=AffineTransform::New();
        affineTransform->SetIdentity();
        affineTransform->SetFixedParameters(centre);
        LinearInternalInterpolatorPtr linearInternal=LinearInternalInterpolator::New();
        
        MeanSquaresMetricPtr metric=MeanSquaresMetric::New();
        InternalImagePtr reducedImage=fixedPyramid->GetOutput(0);
        {
            std::ostringstream oss;
            oss << "Size of top of pyramid: " << reducedImage->GetLargestPossibleRegion().GetSize();
            m_log(2,oss.str());
        }
        Region reducedImageRegion;
        {
            auto schedule = fixedPyramid->GetSchedule();
            const Size inputSize=iFixedImage->GetLargestPossibleRegion().GetSize();
            const Index inputStart=iFixedImage->GetLargestPossibleRegion().GetIndex();
            Size  size;
            Index start;
            for ( unsigned int dim = 0; dim < Dimension; ++dim) {
                const float scaleFactor = static_cast<float>( schedule[ 0 ][ dim ] );
                size[ dim ] = static_cast<typename Size::SizeValueType>(
                    vcl_floor(static_cast<float>( inputSize[ dim ] ) / scaleFactor ) );
                if( size[ dim ] < 1 ) { size[ dim ] = 1;  }
                start[ dim ] = static_cast<typename Index::IndexValueType>(
                    vcl_ceil(static_cast<float>( inputStart[ dim ] ) / scaleFactor ) ); 
            }
            reducedImageRegion.SetSize(size);
            reducedImageRegion.SetIndex(start);
        }
        {
            std::ostringstream oss;
            oss << "Reduced Image region: " << reducedImageRegion.GetIndex() << " size "<< reducedImageRegion.GetSize();
            m_log(2,oss.str());
        }

        
        metric->SetMovingImage(movingPyramid->GetOutput(0));
        metric->SetFixedImage(fixedPyramid->GetOutput(0));
        metric->SetFixedImageRegion(reducedImageRegion);
        metric->SetTransform(affineTransform);
        
        metric->SetInterpolator(linearInternal);
        
        metric->Initialize();
        
        RSGDPtr optimizer=RSGD::New();
        optimizer->SetCostFunction(metric);
        //Todo: use principled method here.
        Parameters initialParameters=affineTransform->GetParameters();
        Parameters optimizerScales(affineTransform->GetNumberOfParameters());
        optimizerScales.Fill(72.*72);
        optimizerScales[9]=1.0;
        optimizerScales[10]=1.0;
        optimizerScales[11]=1.0;
        optimizer->SetScales(optimizerScales);
        optimizer->SetInitialPosition(initialParameters);
        {
            std::ostringstream oss;
            oss << "Initial parameters: " << initialParameters;
            m_log(2,oss.str());
        }

        // do the optimization
        optimizer->StartOptimization();

        // get the results
        Parameters lastTransformParameters = optimizer->GetCurrentPosition();
        affineTransform->SetParameters( lastTransformParameters );
        
        {
            std::ostringstream oss;
            oss << "Found optimal parameters " << lastTransformParameters;
            m_log(2,oss.str());
        }
        oDisplacementField->CopyInformation(iFixedImage);
        oDisplacementField->Allocate();
        itk::ImageRegionIteratorWithIndex<DisplacementField> fieldIter(oDisplacementField,oDisplacementField->GetLargestPossibleRegion());

        for(fieldIter.GoToBegin();!fieldIter.IsAtEnd();++fieldIter)
	{
		Point p;
		oDisplacementField->TransformIndexToPhysicalPoint(fieldIter.GetIndex(),p);
                Point q=affineTransform->TransformPoint(p);
                fieldIter.Set(q-p);
	}

        // if output requested, resample image and save transform
        if (!m_outputpath.empty()) {
            TransformWriterPtr twriter=TransformWriter::New();
            twriter->SetFileName(m_outputpath+"BaseRegistrationTransform.tfm");
            twriter->SetInput(affineTransform);
            twriter->Update();
            
            auto filter=itk::MinimumMaximumImageFilter<Image>::New();
            filter->SetInput(iFixedImage);
            filter->Update();
            PixelType theMinimum = filter->GetMinimum();
            {
                std::ostringstream oss;
                oss << "Found minimum value " << theMinimum;
                m_log(2,oss.str());
            }
            

            ResamplerPtr resampler=Resampler::New();
            resampler->SetInput(iMovingImage);
            resampler->SetTransform(affineTransform);
            LinearInterpolatorPtr linear=LinearInterpolator::New();
            resampler->SetInterpolator(linear);
            resampler->SetReferenceImage(iFixedImage);
            resampler->UseReferenceImageOn();
            resampler->SetDefaultPixelValue(theMinimum);
            
            WriterPtr writer=Writer::New();
            writer->SetFileName(m_outputpath+"BaseRegistrationResult.mha");
            writer->SetInput(resampler->GetOutput());
            writer->Update();
        }
 
        
    }
   }; 
