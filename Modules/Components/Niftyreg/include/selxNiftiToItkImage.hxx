/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "selxNiftiToItkImage.h"
#include "itkMath.h"
//#include "itkIOCommon.h"
//#include "itkMetaDataObject.h"
//#include "itkSpatialOrientationAdapter.h"
#include "itkImportImageFilter.h"
namespace selx
{


  template<class ItkImageType, class NiftiPixelType>
  typename ItkImageType::Pointer
    NiftiToItkImage< ItkImageType, NiftiPixelType >
    ::Convert(std::shared_ptr<nifti_image> input_image)
  {

    auto imageInformationFromNifti = NiftiToItkImage< ItkImageType, NiftiPixelType >::ReadImageInformation(input_image);

    auto importImageFilter = itk::ImportImageFilter<ItkImageType::PixelType, ItkImageType::ImageDimension>::New();
    
    //importImageFilter->SetRegion(fixedImageDomain->GetLargestPossibleRegion());
    //importImageFilter->SetOrigin(fixedImageDomain->GetOrigin());
    //importImageFilter->SetSpacing(fixedImageDomain->GetSpacing());
    //importImageFilter->SetDirection(fixedImageDomain->GetDirection());
    //importImageFilter->UpdateOutputInformation();


    return importImageFilter->GetOutput();

  }
  template<class ItkImageType, class NiftiPixelType>
  bool
    NiftiToItkImage< ItkImageType, NiftiPixelType >
    ::MustRescale(double rescaleSlope, double rescaleIntercept)
  {
    return std::abs(rescaleSlope) > std::numeric_limits< double >::epsilon()
      && (std::abs(rescaleSlope - 1.0) > std::numeric_limits< double >::epsilon()
      || std::abs(rescaleIntercept) > std::numeric_limits< double >::epsilon());
  }

  // Internal function to rescale pixel according to Rescale Slope/Intercept
  template< typename TBuffer >
  void RescaleFunction(TBuffer *buffer,
    double slope,
    double intercept,
    size_t size)
  {
    for (size_t i = 0; i < size; i++)
    {
      double tmp = static_cast<double>(buffer[i]) * slope;
      tmp += intercept;
      buffer[i] = static_cast<TBuffer>(tmp);
    }
  }

  template< typename PixelType >
  void
    CastCopy(float *to, void *from, size_t pixelcount)
  {
    PixelType *_from = static_cast<PixelType *>(from);

    for (unsigned i = 0; i < pixelcount; i++)
    {
      to[i] = static_cast<float>(_from[i]);
    }
  }

  template<class ItkImageType, class NiftiPixelType>
  void
    NiftiToItkImage< ItkImageType, NiftiPixelType >
    ::Read(void *buffer, typename ItkImageType::Pointer output_image, std::shared_ptr<nifti_image> input_image )
  {
    void *data = ITK_NULLPTR;

    ImageIORegion            regionToRead = output_image->GetIORegion();
    ImageIORegion::SizeType  size = regionToRead.GetSize();
    ImageIORegion::IndexType start = regionToRead.GetIndex();

    size_t       numElts = 1;
    int          _origin[7];
    int          _size[7];
    unsigned int i;

    for (i = 0; i < start.size(); i++)
    {
      _origin[i] = static_cast<int>(start[i]);
      _size[i] = static_cast<int>(size[i]);
      numElts *= _size[i];
    }
    for (; i < 7; i++)
    {
      _origin[i] = 0;
      _size[i] = 1;
    }

    unsigned int numComponents = ItkImageProperties<ItkImageType>::GetNumberOfComponents();
    //
    // special case for images of vector pixels
    if (numComponents > 1 && ItkImageProperties<ItkImageType>::GetPixelType() != COMPLEX)
    {
      // nifti always sticks vec size in dim 4, so have to shove
      // other dims out of the way
      _size[6] = _size[5];
      _size[5] = _size[4];
      // sizes = x y z t vecsize
      _size[4] = numComponents;
    }

    if (input_image == ITK_NULLPTR)
    {

    }

    //
    // decide whether to read whole region or subregion, by stepping
    // thru dims and comparing them to requested sizes
    for (i = 0; i < ItkImageProperties<ItkImageType>::GetNumberOfDimensions(); i++)
    {
      if (input_image->dim[i + 1] != _size[i])
      {
        break;
      }
    }
    // if all dimensions match requested size, just read in
    // all data as a block
    if (i == ItkImageProperties<ItkImageType>::GetNumberOfDimensions())
    {
      if (nifti_image_load(input_image) == -1)
      {
        itkExceptionMacro(<< "nifti_image_load failed for file: "
          << this->GetFileName());
      }
      data = input_image->data;
    }
    else
    {
      // read in a subregion
      if (nifti_read_subregion_image(input_image,
        _origin,
        _size,
        &data) == -1)
      {
        itkExceptionMacro(<< "nifti_read_subregion_image failed for file: "
          << this->GetFileName());
      }
    }
    unsigned int pixelSize = input_image->nbyper;
    //
    // if we're going to have to rescale pixels, and the on-disk
    // pixel type is different than the pixel type reported to
    // ImageFileReader, we have to up-promote the data to float
    // before doing the rescale.
    //
    if (MustRescale(rescaleSlope, rescaleIntercept)
      && imageBaseProperties.componentType != this->m_OnDiskComponentType)
    {
      pixelSize =
        static_cast<unsigned int>(ItkImageProperties<ItkImageType>::GetNumberOfComponents())
        * static_cast<unsigned int>(sizeof(float));

      // Deal with correct management of 64bits platforms
      const size_t imageSizeInComponents =
        static_cast<size_t>(this->GetImageSizeInComponents());

      //
      // allocate new buffer for floats. Malloc instead of new to
      // be consistent with allocation used in niftilib
      float *_data =
        static_cast<float *>
        (malloc(imageSizeInComponents * sizeof(float)));
      switch (this->m_OnDiskComponentType)
      {
      case CHAR:
        CastCopy< char >(_data, data, imageSizeInComponents);
        break;
      case UCHAR:
        CastCopy< unsigned char >(_data, data, imageSizeInComponents);
        break;
      case SHORT:
        CastCopy< short >(_data, data, imageSizeInComponents);
        break;
      case USHORT:
        CastCopy< unsigned short >(_data, data, imageSizeInComponents);
        break;
      case INT:
        CastCopy< int >(_data, data, imageSizeInComponents);
        break;
      case UINT:
        CastCopy< unsigned int >(_data, data, imageSizeInComponents);
        break;
      case LONG:
        CastCopy< long >(_data, data, imageSizeInComponents);
        break;
      case ULONG:
        CastCopy< unsigned long >(_data, data, imageSizeInComponents);
        break;
      case FLOAT:
        itkExceptionMacro(<< "FLOAT pixels do not need Casting to float");
      case DOUBLE:
        itkExceptionMacro(<< "DOUBLE pixels do not need Casting to float");
      case UNKNOWNCOMPONENTTYPE:
        itkExceptionMacro(<< "Bad OnDiskComponentType UNKNOWNCOMPONENTTYPE");
      }
      //
      // we're replacing the data pointer, so if it was allocated
      // in nifti_read_subregion_image, free the old data here
      if (data != input_image->data)
      {
        free(data);
      }
      data = _data;
    }
    //
    // if single or complex, nifti layout == itk layout
    if (numComponents == 1
      || ItkImageProperties<ItkImageType>::GetPixelType() == COMPLEX
      || ItkImageProperties<ItkImageType>::GetPixelType() == RGB
      || ItkImageProperties<ItkImageType>::GetPixelType() == RGBA)
    {
      const size_t NumBytes = numElts * pixelSize;
      memcpy(buffer, data, NumBytes);
      //
      // if read_subregion was called it allocates a buffer that needs to be
      // freed.
      if (data != input_image->data)
      {
        free(data);
      }
    }
    else
    {
      // otherwise nifti is x y z t vec l m 0, itk is
      // vec x y z t l m o
      const char *       niftibuf = (const char *)data;
      char *             itkbuf = (char *)buffer;
      const size_t rowdist = input_image->dim[1];
      const size_t slicedist = rowdist * input_image->dim[2];
      const size_t volumedist = slicedist * input_image->dim[3];
      const size_t seriesdist = volumedist * input_image->dim[4];
      //
      // as per ITK bug 0007485
      // NIfTI is lower triangular, ITK is upper triangular.
      int *vecOrder;
      if (ItkImageProperties<ItkImageType>::GetPixelType() == ImageIOBase::DIFFUSIONTENSOR3D
        || ItkImageProperties<ItkImageType>::GetPixelType() == ImageIOBase::SYMMETRICSECONDRANKTENSOR)
      {
        //      vecOrder = LowerToUpperOrder(SymMatDim(numComponents));
        vecOrder = UpperToLowerOrder(SymMatDim(numComponents));
      }
      else
      {
        vecOrder = new int[numComponents];
        for (i = 0; i < numComponents; i++)
        {
          vecOrder[i] = i;
        }
      }
      for (int t = 0; t < input_image->dim[4]; t++)
      {
        for (int z = 0; z < input_image->dim[3]; z++)
        {
          for (int y = 0; y < input_image->dim[2]; y++)
          {
            for (int x = 0; x < input_image->dim[1]; x++)
            {
              for (unsigned int c = 0; c < numComponents; c++)
              {
                const size_t nifti_index =
                  (c * seriesdist + volumedist * t + slicedist * z + rowdist * y + x) * pixelSize;
                const size_t itk_index =
                  ((volumedist * t + slicedist * z + rowdist * y + x) * numComponents + vecOrder[c]) * pixelSize;
                for (unsigned int b = 0; b < pixelSize; ++b)
                {
                  itkbuf[itk_index + b] = niftibuf[nifti_index + b];
                }

              }
            }
          }
        }
      }
      delete[] vecOrder;
      dumpdata(data);
      dumpdata(buffer);
      // if read_subregion was called it allocates a buffer that needs to be
      // freed.
      if (data != input_image->data)
      {
        free(data);
      }
    }

    // If the scl_slope field is nonzero, then rescale each voxel value in the
    // dataset.
    // Complete description of can be found in nifti1.h under "DATA SCALING"
    if (MustRescale(rescaleSlope, rescaleIntercept))
    {
      switch (ItkImageProperties<ItkImageType>::GetComponentType())
      {
      case CHAR:
        RescaleFunction(static_cast<char *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case UCHAR:
        RescaleFunction(static_cast<unsigned char *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case SHORT:
        RescaleFunction(static_cast<short *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case USHORT:
        RescaleFunction(static_cast<unsigned short *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case INT:
        RescaleFunction(static_cast<int *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case UINT:
        RescaleFunction(static_cast<unsigned int *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case LONG:
        RescaleFunction(static_cast<long *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case ULONG:
        RescaleFunction(static_cast<unsigned long *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case FLOAT:
        RescaleFunction(static_cast<float *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      case DOUBLE:
        RescaleFunction(static_cast<double *>(buffer),
          rescaleSlope,
          rescaleIntercept, numElts);
        break;
      default:
        if (this->GetPixelType() == SCALAR)
        {
          itkExceptionMacro(<< "Datatype: "
            << itk::ImageIOBase::GetComponentTypeAsString(ItkImageProperties<ItkImageType>::GetComponentType())
            << " not supported");
        }
      }
    }
  }

  // This method adds the available header information to the
  // metadata dictionary.
  template<class ItkImageType, class NiftiPixelType>
  static void
    NiftiToItkImage< ItkImageType, NiftiPixelType >
    ::SetImageIOMetadataFromNIfTI(std::shared_ptr<nifti_image> input, itk::MetaDataDictionary& thisDic)
  {
       // Encapsulate as many header information as possible.
      //
      // Necessary to clear dict if ImageIO object is re-used
      // thisDic.Clear();

      std::ostringstream dim_info;
      dim_info << input->dim_info;
      itk::EncapsulateMetaData< std::string >(thisDic, "dim_info", dim_info.str());

      for (int idx = 0; idx < 8; idx++)
      {
        std::ostringstream dim;
        dim << input->dim[idx];
        std::ostringstream dimKey;
        dimKey << "dim[" << idx << "]";
        itk::EncapsulateMetaData< std::string >(thisDic, dimKey.str(), dim.str());
      }

      std::ostringstream intent_p1;
      intent_p1 << input->intent_p1;
      itk::EncapsulateMetaData< std::string >(thisDic, "intent_p1", intent_p1.str());

      std::ostringstream intent_p2;
      intent_p2 << input->intent_p2;
      itk::EncapsulateMetaData< std::string >(thisDic, "intent_p2", intent_p2.str());

      std::ostringstream intent_p3;
      intent_p3 << input->intent_p3;
      itk::EncapsulateMetaData< std::string >(thisDic, "intent_p3", intent_p3.str());

      std::ostringstream intent_code;
      intent_code << input->intent_code;
      itk::EncapsulateMetaData< std::string >(thisDic, "intent_code", intent_code.str());

      std::ostringstream datatype;
      datatype << input->datatype;
      itk::EncapsulateMetaData< std::string >(thisDic, "datatype", datatype.str());

      std::ostringstream bitpix;
      bitpix << input->bitpix;
      itk::EncapsulateMetaData< std::string >(thisDic, "bitpix", bitpix.str());

      std::ostringstream slice_start;
      slice_start << input->slice_start;
      itk::EncapsulateMetaData< std::string >(thisDic, "slice_start", slice_start.str());

      for (int idx = 0; idx < 8; idx++)
      {
        std::ostringstream pixdim;
        pixdim << input->pixdim[idx];
        std::ostringstream pixdimKey;
        pixdimKey << "pixdim[" << idx << "]";
        itk::EncapsulateMetaData< std::string >(thisDic, pixdimKey.str(), pixdim.str());
      }

      std::ostringstream vox_offset;
      vox_offset << input->vox_offset;
      itk::EncapsulateMetaData< std::string >(thisDic, "vox_offset", vox_offset.str());

      std::ostringstream scl_slope;
      scl_slope << input->scl_slope;
      itk::EncapsulateMetaData< std::string >(thisDic, "scl_slope", scl_slope.str());

      std::ostringstream scl_inter;
      scl_inter << input->scl_inter;
      itk::EncapsulateMetaData< std::string >(thisDic, "scl_inter", scl_inter.str());

      std::ostringstream slice_end;
      slice_end << input->slice_end;
      itk::EncapsulateMetaData< std::string >(thisDic, "slice_end", slice_end.str());

      std::ostringstream slice_code;
      slice_code << input->slice_code;
      itk::EncapsulateMetaData< std::string >(thisDic, "slice_code", slice_code.str());

      std::ostringstream xyzt_units;
      xyzt_units << input->xyzt_units;
      itk::EncapsulateMetaData< std::string >(thisDic, "xyzt_units", xyzt_units.str());

      std::ostringstream cal_max;
      cal_max << input->cal_max;
      itk::EncapsulateMetaData< std::string >(thisDic, "cal_max", cal_max.str());

      std::ostringstream cal_min;
      cal_min << input->cal_min;
      itk::EncapsulateMetaData< std::string >(thisDic, "cal_min", cal_min.str());

      std::ostringstream slice_duration;
      slice_duration << input->slice_duration;
      itk::EncapsulateMetaData< std::string >(thisDic, "slice_duration", slice_duration.str());

      std::ostringstream toffset;
      toffset << input->toffset;
      itk::EncapsulateMetaData< std::string >(thisDic, "toffset", toffset.str());

      std::ostringstream descrip;
      descrip << input->descrip;
      itk::EncapsulateMetaData< std::string >(thisDic, "descrip", descrip.str());

      std::ostringstream aux_file;
      aux_file << input->aux_file;
      itk::EncapsulateMetaData< std::string >(thisDic, "aux_file", aux_file.str());

      std::ostringstream qform_code;
      qform_code << input->qform_code;
      itk::EncapsulateMetaData< std::string >(thisDic, "qform_code", qform_code.str());

      std::ostringstream sform_code;
      sform_code << input->sform_code;
      itk::EncapsulateMetaData< std::string >(thisDic, "sform_code", sform_code.str());

      std::ostringstream quatern_b;
      quatern_b << input->quatern_b;
      itk::EncapsulateMetaData< std::string >(thisDic, "quatern_b", quatern_b.str());

      std::ostringstream quatern_c;
      quatern_c << input->quatern_c;
      itk::EncapsulateMetaData< std::string >(thisDic, "quatern_c", quatern_c.str());

      std::ostringstream quatern_d;
      quatern_d << input->quatern_d;
      itk::EncapsulateMetaData< std::string >(thisDic, "quatern_d", quatern_d.str());

      std::ostringstream qoffset_x;
      qoffset_x << input->qoffset_x;
      itk::EncapsulateMetaData< std::string >(thisDic, "qoffset_x", qoffset_x.str());

      std::ostringstream qoffset_y;
      qoffset_y << input->qoffset_y;
      itk::EncapsulateMetaData< std::string >(thisDic, "qoffset_y", qoffset_y.str());

      std::ostringstream qoffset_z;
      qoffset_z << input->qoffset_z;
      itk::EncapsulateMetaData< std::string >(thisDic, "qoffset_z", qoffset_z.str());

      std::ostringstream srow_x;
      srow_x << input->srow_x[0] << " " << input->srow_x[1] << " " << input->srow_x[2] << " " << input->srow_x[3];
      itk::EncapsulateMetaData< std::string >(thisDic, "srow_x", srow_x.str());

      std::ostringstream srow_y;
      srow_y << input->srow_y[0] << " " << input->srow_y[1] << " " << input->srow_y[2] << " " << input->srow_y[3];
      itk::EncapsulateMetaData< std::string >(thisDic, "srow_y", srow_y.str());

      std::ostringstream srow_z;
      srow_z << input->srow_z[0] << " " << input->srow_z[1] << " " << input->srow_z[2] << " " << input->srow_z[3];
      itk::EncapsulateMetaData< std::string >(thisDic, "srow_z", srow_z.str());

      std::ostringstream intent_name;
      intent_name << input->intent_name;
      itk::EncapsulateMetaData< std::string >(thisDic, "intent_name", intent_name.str());
    
      return thisDic;
  }

  template<class ItkImageType, class NiftiPixelType>
  ImageInformationFromNifti
    NiftiToItkImage< ItkImageType, NiftiPixelType >
    ::ReadImageInformation(std::shared_ptr<nifti_image> input)
  {

    ImageInformationFromNifti imageInformationFromNifti = {
      1, // double rescaleSlope;
      0, // double rescaleIntercept;
      0, // unsigned int numberOfDimensions; 
      0, // unsigned int numberOfComponents;
      IOPixelType::UNKNOWNPIXELTYPE, // IOPixelType pixelType;
      IOComponentType::UNKNOWNCOMPONENTTYPE,// IOComponentType componentType;
      {}, //std::vector<double> dimensions;
      {} //std::vector<double> spacing;
    };

    if (input == ITK_NULLPTR)
    {
    //  itkExceptionMacro(<< this->GetFileName() << " is not recognized as a NIFTI file");
    }
    //Check the intent code, it is a vector image, or matrix image, then this is
    // not true.
    //
    if (input->intent_code == NIFTI_INTENT_VECTOR
      || input->intent_code == NIFTI_INTENT_SYMMATRIX)
    {
      if (input->dim[4] > 1)
      {
        imageInformationFromNifti.numberOfDimensions = 4;
      }
      else if (input->dim[3] > 1)
      {
        imageInformationFromNifti.numberOfDimensions = 3;
      }
      else if (input->dim[2] > 1)
      {
        imageInformationFromNifti.numberOfDimensions = 2;
      }
      else
      {
        imageInformationFromNifti.numberOfDimensions = 1;
      }
    }
    else if (input->intent_code == NIFTI_INTENT_GENMATRIX)
    { //TODO:  NEED TO DEAL WITH CASE WHERE NIFTI_INTENT_MATRIX
      itkGenericExceptionMacro(
        << "The input of NiftiToItkImage has an intent code of NIFTI_INTENT_GENMATRIX which is not yet implemented in ITK");
    }
    else
    { //Simple Scalar Image
      //
      //    this->SetNumberOfDimensions(input->dim[0]);
      // HACK ALERT KW
      // Apparently some straight-from-the-scanner files report as 4D
      // with T = 1; this causes ImageFileReader to erroneously ignore the
      // reported
      // direction cosines.
      unsigned realdim;
      for (realdim = input->dim[0];
        input->dim[realdim] == 1 && realdim > 3;
        realdim--)
      {
      }
      imageInformationFromNifti.numberOfDimensions = realdim ;
      imageInformationFromNifti.numberOfComponents = 1;
    }

    if (input->intent_code == NIFTI_INTENT_VECTOR
      || input->intent_code == NIFTI_INTENT_SYMMATRIX)
    {
      imageInformationFromNifti.numberOfComponents = input->dim[5];
    }
    else if (input->intent_code == NIFTI_INTENT_GENMATRIX)
    { //TODO:  NEED TO DEAL WITH CASE WHERE NIFTI_INTENT_MATRIX
      itkGenericExceptionMacro(
        << "The input of NiftiToItkImage has an intent code of NIFTI_INTENT_GENMATRIX which is not yet implemented in ITK");
    }
    //TODO:  Dealing with NIFTI_INTENT_VECTOR or NIFTI_INTENT_GENMATRIX with data
    // type of NIFTI_TYPE_COMPLEX64 NIFTI_TYPE_COMPLEX128 NIFTI_TYPE_RGB24 not
    // supported.

    switch (input->datatype)
    {
    case NIFTI_TYPE_INT8:
      imageInformationFromNifti.componentType = IOComponentType::CHAR;
      imageInformationFromNifti.pixelType = IOPixelType::SCALAR;
      break;
    case NIFTI_TYPE_UINT8:
      imageInformationFromNifti.componentType = IOComponentType::UCHAR;
      imageInformationFromNifti.pixelType = IOPixelType::SCALAR;
      break;
    case NIFTI_TYPE_INT16:
      imageInformationFromNifti.componentType = IOComponentType::SHORT;
      imageInformationFromNifti.pixelType = IOPixelType::SCALAR;
      break;
    case NIFTI_TYPE_UINT16:
      imageInformationFromNifti.componentType = IOComponentType::USHORT;
      imageInformationFromNifti.pixelType = IOPixelType::SCALAR;
      break;
    case NIFTI_TYPE_INT32:
      imageInformationFromNifti.componentType = IOComponentType::INT;
      imageInformationFromNifti.pixelType = IOPixelType::SCALAR;
      break;
    case NIFTI_TYPE_UINT32:
      imageInformationFromNifti.componentType = IOComponentType::UINT;
      imageInformationFromNifti.pixelType = IOPixelType::SCALAR;
      break;
    case NIFTI_TYPE_FLOAT32:
      imageInformationFromNifti.componentType = IOComponentType::FLOAT;
      imageInformationFromNifti.pixelType = IOPixelType::SCALAR;
      break;
    case NIFTI_TYPE_FLOAT64:
      imageInformationFromNifti.componentType = IOComponentType::DOUBLE;
      imageInformationFromNifti.pixelType = IOPixelType::SCALAR;
      break;
    case NIFTI_TYPE_COMPLEX64:
      imageInformationFromNifti.componentType = IOComponentType::FLOAT;
      imageInformationFromNifti.pixelType = IOPixelType::COMPLEX;
      imageInformationFromNifti.numberOfComponents = (2);
      break;
    case NIFTI_TYPE_COMPLEX128:
      imageInformationFromNifti.componentType = IOComponentType::DOUBLE;
      imageInformationFromNifti.pixelType = IOPixelType::COMPLEX;
      imageInformationFromNifti.numberOfComponents = (2);
      break;
    case NIFTI_TYPE_RGB24:
      imageInformationFromNifti.componentType = IOComponentType::UCHAR;
      imageInformationFromNifti.pixelType = IOPixelType::RGB;
      imageInformationFromNifti.numberOfComponents = (3);
      //TODO:  Need to be able to read/write RGB images into ITK.
      //    case DT_RGB:
      // DEBUG -- Assuming this is a triple, not quad
      //image.setDataType( uiig::DATA_RGBQUAD );
      break;
    case NIFTI_TYPE_RGBA32:
      imageInformationFromNifti.componentType = IOComponentType::UCHAR;
      imageInformationFromNifti.pixelType = IOPixelType::RGBA;
      imageInformationFromNifti.numberOfComponents = (4);
      break;
    default:
      break;
    }

    // there are a wide variety of intents we ignore
    // but a few wee need to care about
    switch (input->intent_code)
    {
    case NIFTI_INTENT_SYMMATRIX:
      imageInformationFromNifti.pixelType = IOPixelType::SYMMETRICSECONDRANKTENSOR;
      break;
    case NIFTI_INTENT_VECTOR:
      imageInformationFromNifti.pixelType = IOPixelType::VECTOR;
      break;
    case NIFTI_INTENT_NONE:
    case NIFTI_INTENT_CORREL:
    case NIFTI_INTENT_TTEST:
    case NIFTI_INTENT_FTEST:
    case NIFTI_INTENT_ZSCORE:
    case NIFTI_INTENT_CHISQ:
    case NIFTI_INTENT_BETA:
    case NIFTI_INTENT_BINOM:
    case NIFTI_INTENT_GAMMA:
    case NIFTI_INTENT_POISSON:
    case NIFTI_INTENT_NORMAL:
    case NIFTI_INTENT_FTEST_NONC:
    case NIFTI_INTENT_CHISQ_NONC:
    case NIFTI_INTENT_LOGISTIC:
    case NIFTI_INTENT_LAPLACE:
    case NIFTI_INTENT_UNIFORM:
    case NIFTI_INTENT_TTEST_NONC:
    case NIFTI_INTENT_WEIBULL:
    case NIFTI_INTENT_CHI:
    case NIFTI_INTENT_INVGAUSS:
    case NIFTI_INTENT_EXTVAL:
    case NIFTI_INTENT_PVAL:
    case NIFTI_INTENT_LOGPVAL:
    case NIFTI_INTENT_LOG10PVAL:
    case NIFTI_INTENT_ESTIMATE:
    case NIFTI_INTENT_LABEL:
    case NIFTI_INTENT_NEURONAME:
    case NIFTI_INTENT_GENMATRIX:
    case NIFTI_INTENT_DISPVECT:
    case NIFTI_INTENT_POINTSET:
    case NIFTI_INTENT_TRIANGLE:
    case NIFTI_INTENT_QUATERNION:
    case NIFTI_INTENT_DIMLESS:
    case NIFTI_INTENT_TIME_SERIES:
    case NIFTI_INTENT_NODE_INDEX:
    case NIFTI_INTENT_RGB_VECTOR:
    case NIFTI_INTENT_RGBA_VECTOR:
    case NIFTI_INTENT_SHAPE:
    default:
      break;
    }

    // set slope/intercept
    if (input->qform_code == 0
      && input->sform_code == 0)
    {
      imageInformationFromNifti.rescaleSlope = 1;
      imageInformationFromNifti.rescaleIntercept = 0;
    }
    else
    {
      imageInformationFromNifti.rescaleSlope = input->scl_slope;
      if (std::abs(imageInformationFromNifti.rescaleSlope) < itk::NumericTraits<double>::epsilon())
      {
        imageInformationFromNifti.rescaleSlope = 1;
      }
      imageInformationFromNifti.rescaleIntercept = input->scl_inter;
    }

    /* this->m_OnDiskComponentType = imageInformationFromNifti.componentType;
    //
    // if rescale is necessary, promote type reported
    // to ImageFileReader to float
    if (MustRescale(imageInformationFromNifti.rescaleSlope, imageInformationFromNifti.rescaleIntercept))
    {
      if (imageInformationFromNifti.componentType == IOComponentType::CHAR
        || imageInformationFromNifti.componentType == IOComponentType::UCHAR
        || imageInformationFromNifti.componentType == IOComponentType::SHORT
        || imageInformationFromNifti.componentType == IOComponentType::USHORT
        || imageInformationFromNifti.componentType == IOComponentType::INT
        || imageInformationFromNifti.componentType == IOComponentType::UINT
        || imageInformationFromNifti.componentType == IOComponentType::LONG
        || imageInformationFromNifti.componentType == IOComponentType::ULONG)
      {
        imageInformationFromNifti.componentType = IOComponentType::FLOAT;
      }
    }
    */
    //
    // set up the dimension stuff
    double spacingscale = 1.0; //default to mm
    switch (XYZT_TO_SPACE(input->xyz_units))
    {
    case NIFTI_UNITS_METER:
      spacingscale = 1e3;
      break;
    case NIFTI_UNITS_MM:
      spacingscale = 1e0;
      break;
    case NIFTI_UNITS_MICRON:
      spacingscale = 1e-3;
      break;
    }
    double timingscale = 1.0; //Default to seconds
    switch (XYZT_TO_TIME(input->xyz_units))
    {
    case NIFTI_UNITS_SEC:
      timingscale = 1.0;
      break;
    case NIFTI_UNITS_MSEC:
      timingscale = 1e-3;
      break;
    case NIFTI_UNITS_USEC:
      timingscale = 1e-6;
      break;
    }
    imageInformationFromNifti.dimensions.resize(imageInformationFromNifti.numberOfDimensions);
    imageInformationFromNifti.spacing.resize(imageInformationFromNifti.numberOfDimensions);

    switch (imageInformationFromNifti.numberOfDimensions)
    {
    case 7:
      imageInformationFromNifti.dimensions[6] = input->nw;
      //NOTE: Scaling is not defined in this dimension
      imageInformationFromNifti.spacing[6] = input->dw;
      ITK_FALLTHROUGH;
    case 6:
      imageInformationFromNifti.dimensions[5] = input->nv;
      //NOTE: Scaling is not defined in this dimension
      imageInformationFromNifti.spacing[5] = input->dv;
      ITK_FALLTHROUGH;
    case 5:
      imageInformationFromNifti.dimensions[4] = input->nu;
      //NOTE: Scaling is not defined in this dimension
      imageInformationFromNifti.spacing[4] = input->du;
      ITK_FALLTHROUGH;
    case 4:
      imageInformationFromNifti.dimensions[3] = input->nt;
      imageInformationFromNifti.spacing[3] = input->dt * timingscale;
      ITK_FALLTHROUGH;
    case 3:
      imageInformationFromNifti.dimensions[2] = input->nz;
      imageInformationFromNifti.spacing[2] = input->dz * spacingscale;
      ITK_FALLTHROUGH;
    case 2:
      imageInformationFromNifti.dimensions[1] = input->ny;
      imageInformationFromNifti.spacing[1] = input->dy * spacingscale;
      ITK_FALLTHROUGH;
    case 1:
      imageInformationFromNifti.dimensions[0] = input->nx;
      imageInformationFromNifti.spacing[0] = input->dx * spacingscale;
      break;
    default:
      itkGenericExceptionMacro(
        << "The input of NiftiToItkImage has " << imageInformationFromNifti.numberOfDimensions << " dimensions, and is not supported or invalid!");
    }

    //floris TODO:
    //this->ComputeStrides();
    /*
    m_Strides[0] = this->GetComponentSize();
    m_Strides[1] = m_NumberOfComponents * m_Strides[0];
    for (unsigned int i = 2; i <= (m_NumberOfDimensions + 1); i++)
    {
      m_Strides[i] = static_cast<SizeType>(m_Dimensions[i - 2]) * m_Strides[i - 1];
    }
    */

    //Create Dictionary Information
    MetaDataDictionary thisDic = std::make_unique< itk::MetaDataDictionary>;

    // Set the metadata.
    std::string          classname("selxNiftiToItkImage");
    itk::EncapsulateMetaData< std::string >(*thisDic, "ITK_InputFilterName", classname);

    //Need to encapsulate as much Nifti information as possible here.
    SetImageIOMetadataFromNIfTI(input, *thisDic);

    // set the image orientation
    SetImageIOOrientationFromNIfTI(dims, input, *thisDic);

    //Important hist fields
    std::string description(input->descrip);
    itk::EncapsulateMetaData< std::string >(*thisDic,
      "ITK_FileNotes", description);

    // We don't need the image anymore
    //nifti_image_free(input_image);
    //input_image = ITK_NULLPTR;
    return imageInformationFromNifti;
  }

  namespace
  {
    void Normalize(std::vector< double > & x)
    {
      double sum = 0;

      for (unsigned int i = 0; i < x.size(); i++)
      {
        sum += (x[i] * x[i]);
      }
      if (sum == 0.0)
      {
        return;
      }
      sum = std::sqrt(sum);
      for (unsigned int i = 0; i < x.size(); i++)
      {
        x[i] = x[i] / sum;
      }
    }
  }

  template<class ItkImageType, class NiftiPixelType>
  void
    NiftiToItkImage< ItkImageType, NiftiPixelType >::SetImageIOOrientationFromNIfTI(unsigned short int dims, std::shared_ptr<nifti_image> input_NiftiImage, itk::MetaDataDictionary& dict)
  {
    typedef itk::SpatialOrientationAdapter OrientAdapterType;

    //
    // in the case of an Analyze75 file, use old analyze orient method.
    if (input_NiftiImage->qform_code == 0
      && input_NiftiImage->sform_code == 0)
    {
      OrientAdapterType::DirectionType   dir;
      OrientAdapterType::OrientationType orient;
      switch (input_NiftiImage->analyze75_orient)
      {
      case a75_transverse_unflipped:
        orient = SpatialOrientation::ITK_COORDINATE_ORIENTATION_RPI;
        break;
      case a75_sagittal_unflipped:
        orient = SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIR;
        break;
      case a75_coronal_unflipped:
        orient = SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP;
        break;
      case a75_transverse_flipped:
        orient = SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI;
        break;
      case a75_sagittal_flipped:
        orient = SpatialOrientation::ITK_COORDINATE_ORIENTATION_PIL;
        break;
      case a75_coronal_flipped:
        orient = SpatialOrientation::ITK_COORDINATE_ORIENTATION_RSP;
        break;
      case a75_orient_unknown:
        orient = SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP;
        break;
      }
      dir = OrientAdapterType().ToDirectionCosines(orient);
      m_Origin[0] = 0;
      if (dims > 1)
      {
        m_Origin[1] = 0;
      }
      if (dims > 2)
      {
        m_Origin[2] = 0;
      }
      return;
    }

    // not an Analyze file.
    // scale image data based on slope/intercept
    //
    // qform or sform
    //
    mat44 theMat;
    if (input_NiftiImage->qform_code > 0)
    {
      theMat = input_NiftiImage->qto_xyz;
    }
    //    else if(this->m_NiftiImage->sform_code > 0)
    else
    {
      theMat = input_NiftiImage->sto_xyz;
    }

    //
    // set origin
    m_Origin[0] = -theMat.m[0][3];
    if (dims > 1)
    {
      m_Origin[1] = -theMat.m[1][3];
    }
    if (dims > 2)
    {
      m_Origin[2] = theMat.m[2][3];
    }

    const int             max_defined_orientation_dims = (dims > 3) ? 3 : dims;
    std::vector< double > xDirection(dims, 0);
    for (int i = 0; i < max_defined_orientation_dims; i++)
    {
      xDirection[i] = theMat.m[i][0];
      if (i < 2)
      {
        xDirection[i] *= -1.0;
      }
    }
    Normalize(xDirection);
    this->SetDirection(0, xDirection);

    if (max_defined_orientation_dims > 1)
    {
      std::vector< double > yDirection(dims, 0);
      for (int i = 0; i < max_defined_orientation_dims; i++)
      {
        yDirection[i] = theMat.m[i][1];
        if (i < 2)
        {
          yDirection[i] *= -1.0;
        }
      }
      Normalize(yDirection);
      this->SetDirection(1, yDirection);
    }

    if (max_defined_orientation_dims > 2)
    {
      std::vector< double > zDirection(dims, 0);
      for (int i = 0; i < max_defined_orientation_dims; i++)
      {
        zDirection[i] = theMat.m[i][2];
        if (i < 2)
        {
          zDirection[i] *= -1.0;
        }
      }
      Normalize(zDirection);
      this->SetDirection(2, zDirection);
    }
  }
} // end namespace itk
