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
#include "selxItkToNiftiImage.h"
#include "itkMath.h"
//#include "itkIOCommon.h"
//#include "itkMetaDataObject.h"
//#include "itkSpatialOrientationAdapter.h"

namespace selx
{

template< class ItkImageType, class NiftiPixelType >
std::shared_ptr< nifti_image >
ItkToNiftiImage< ItkImageType, NiftiPixelType >::Convert( typename ItkImageType::Pointer input )
{
  nifti_set_debug_level( 0 ); // suppress error messages

  // Make sure that the image is the right type
  // configure pixel type

  /*
    if (strcmp(input->GetNameOfClass(), "VectorImage") == 0)
    {
      typedef typename ItkImageType::InternalPixelType VectorImageScalarType;
      this->SetPixelTypeInfo(static_cast<const VectorImageScalarType *>(ITK_NULLPTR));
      typedef typename ItkImageType::AccessorFunctorType AccessorFunctorType;
      this->SetNumberOfComponents(AccessorFunctorType::GetVectorLength(input));
    }
    else
    {
      // Set the pixel and component type; the number of components.
      this->SetPixelTypeInfo(static_cast<const ItkImageType *>(ITK_NULLPTR));
    }
    */

  auto output = nifti_simple_init_nim();

  // Set the image Information before passing data
  SetHeaderInformation( input, output );

  // Get itkImage buffer
  typename ItkImageType::RegionType largestRegion  = input->GetLargestPossibleRegion();
  typename ItkImageType::RegionType bufferedRegion = input->GetBufferedRegion();
  if( bufferedRegion != largestRegion )
  {
    std::stringstream msg;
    msg
        <<
        "Input ItkImage must be fully loaded in memory. E.g. use m_ItkImage->GetSource()->UpdateLargestPossibleRegion() before calling this function."
        "Input ItkImage must be fully loaded in memory. E.g. use m_ItkImage->GetSource()->UpdateLargestPossibleRegion() before calling this function."
        << std::endl;
    throw std::runtime_error( msg.str() );
  }

  const bool wasCopied = TransferImageData(input->GetBufferPointer(), output );

  // If the data was not copied to the nifti_image, it is shared between the ITK image and the nifti_image. Therefore, in that case, 
  // the ITK image is captured by the deleter of the shared_ptr, to ensure the lifetime of the ITK Image is extended to the end of 
  // the nifti_image lifetime. Note that in that case nifti_image_free should free all dynamically allocated memory of nifti_image 
  // except for its data, therefore its data pointer is in that case set to null. 
  return wasCopied ?
    std::shared_ptr< nifti_image >(output, nifti_image_free):
    std::shared_ptr< nifti_image >(output, [input](nifti_image* ptr) { ptr->data = nullptr; nifti_image_free(ptr); });
}


namespace
{
inline mat44
mat44_transpose( mat44 in )
{
  mat44 out;

  for( unsigned int i = 0; i < 4; i++ )
  {
    for( unsigned int j = 0; j < 4; j++ )
    {
      out.m[ i ][ j ] = in.m[ j ][ i ];
    }
  }
  return out;
}
}
template< class ItkImageType, class NiftiPixelType >
void
ItkToNiftiImage< ItkImageType, NiftiPixelType >
::SetHeaderInformation( typename ItkImageType::Pointer input, nifti_image * output )
{
  // Adapted from iktNiftiImageIO::WriteImageInformation

  //  MetaDataDictionary &thisDic=this->GetMetaDataDictionary();
  //
  //
  // First of all we need to not go any further if there's
  // a dimension of the image that won't fit in a 16 bit short.
  for( unsigned int i = 0; i < ItkImageType::ImageDimension; i++ )
  {
    unsigned int curdim( ItkImageProperties< ItkImageType >::GetDimensions( input, i ) );
    if( curdim > static_cast< unsigned int >( itk::NumericTraits< short >::max() ) )
    {
      itkGenericExceptionMacro( << "Dimension(" << i << ") = " << curdim
                                << " is greater than maximum possible dimension "
                                << itk::NumericTraits< short >::max() );
    }
  }

  // fill out the image header.
  if( output == ITK_NULLPTR )
  {
    output = nifti_simple_init_nim();
  }
  //
  // set the filename
  //std::string FName( "selxItkToNiftiImage.nii" );
  const std::string BaseName( "selxItkToNiftiImage" );
  const bool        IsCompressed = false;
  output->nifti_type = NIFTI_FTYPE_NIFTI1_1;

  output->fname = nifti_makehdrname( BaseName.c_str(), output->nifti_type, false, IsCompressed );
  output->iname = nifti_makeimgname( BaseName.c_str(), output->nifti_type, false, IsCompressed );
  //     FIELD         NOTES
  //     -----------------------------------------------------
  //     sizeof_hdr    must be 348
  //     -----------------------------------------------------
  //     dim           dim[0] and dim[1] are always required;
  //                   dim[2] is required for 2-D volumes,
  //                   dim[3] for 3-D volumes, etc.
  output->nvox = 1;
  //Spacial dims in ITK are given in mm.
  //If 4D assume 4thD is in SECONDS, for all of ITK.
  //NOTE: Due to an ambiguity in the nifti specification, some developers
  // external tools believe that the time units must be set, even if there
  // is only one dataset.  Having the time specified for a purly spatial
  // image has no consequence, so go ahead and set it to seconds.
  output->xyz_units = static_cast< int >( NIFTI_UNITS_MM | NIFTI_UNITS_SEC );
  output->dim[ 7 ]  = output->nw = 1;
  output->dim[ 6 ]  = output->nv = 1;
  output->dim[ 5 ]  = output->nu = 1;
  output->dim[ 4 ]  = output->nt = 1;
  output->dim[ 3 ]  = output->nz = 1;
  output->dim[ 2 ]  = output->ny = 1;
  output->dim[ 1 ]  = output->nx = 1;
  switch( ItkImageType::ImageDimension )
  {
    case 7:
      output->dim[ 7 ]    = output->nw
                          = static_cast< int >( ItkImageProperties< ItkImageType >::GetDimensions( input, 6 ) );
      output->pixdim[ 7 ] = output->dw
                          = static_cast< float >( ItkImageProperties< ItkImageType >::GetSpacing( input, 6 ) );
      output->nvox       *= output->dim[ 7 ];
      ITK_FALLTHROUGH;
    case 6:
      output->dim[ 6 ]    = output->nv
                          = ItkImageProperties< ItkImageType >::GetDimensions( input, 5 );
      output->pixdim[ 6 ] = output->dv
                          = static_cast< float >( ItkImageProperties< ItkImageType >::GetSpacing( input, 5 ) );
      output->nvox       *= output->dim[ 6 ];
      ITK_FALLTHROUGH;
    case 5:
      output->dim[ 5 ]
        = output->nu = ItkImageProperties< ItkImageType >::GetDimensions( input, 4 );
      output->pixdim[ 5 ]
                    = output->du = static_cast< float >( ItkImageProperties< ItkImageType >::GetSpacing( input, 4 ) );
      output->nvox *= output->dim[ 5 ];
      ITK_FALLTHROUGH;
    case 4:
      output->dim[ 4 ] = output->nt
                       = ItkImageProperties< ItkImageType >::GetDimensions( input, 3 );
      output->pixdim[ 4 ]
                    = output->dt = static_cast< float >( ItkImageProperties< ItkImageType >::GetSpacing( input, 3 ) );
      output->nvox *= output->dim[ 4 ];
      ITK_FALLTHROUGH;
    case 3:
      output->dim[ 3 ] = output->nz
                       = ItkImageProperties< ItkImageType >::GetDimensions( input, 2 );
      output->pixdim[ 3 ]
                    = output->dz = static_cast< float >( ItkImageProperties< ItkImageType >::GetSpacing( input, 2 ) );
      output->nvox *= output->dim[ 3 ];
      ITK_FALLTHROUGH;
    case 2:
      output->dim[ 2 ] = output->ny
                       = ItkImageProperties< ItkImageType >::GetDimensions( input, 1 );
      output->pixdim[ 2 ]
                    = output->dy = static_cast< float >( ItkImageProperties< ItkImageType >::GetSpacing( input, 1 ) );
      output->nvox *= output->dim[ 2 ];
      ITK_FALLTHROUGH;
    case 1:
      output->dim[ 1 ] = output->nx
                       = ItkImageProperties< ItkImageType >::GetDimensions( input, 0 );
      output->pixdim[ 1 ]
                    = output->dx = static_cast< float >( ItkImageProperties< ItkImageType >::GetSpacing( input, 0 ) );
      output->nvox *= output->dim[ 1 ];
  }

  const unsigned int numComponents = ItkImageProperties< ItkImageType >::GetNumberOfComponents();

  //TODO:  Also need to check for RGB images where numComponets=3
  if( numComponents > 1
    && !( ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::COMPLEX
    &&  numComponents == 2 )
    && !( ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::RGB
    && numComponents == 3 )
    && !( ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::RGBA
    && numComponents == 4 ) )
  {
    output->ndim = 5;   //This must be 5 for NIFTI_INTENT_VECTOR
    // images.
    output->dim[ 0 ] = 5; //This must be 5 for NIFTI_INTENT_VECTOR
    // images.
    if( ItkImageType::ImageDimension > 4 )
    {
      itkGenericExceptionMacro(
          << "Can not store a vector image of more than 4 dimensions in a Nifti file. Dimension="
          << ItkImageType::ImageDimension  );
    }
    //
    // support symmetric matrix type
    if( ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::DIFFUSIONTENSOR3D
      || ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::SYMMETRICSECONDRANKTENSOR )
    {
      output->intent_code = NIFTI_INTENT_SYMMATRIX;
    }
    else
    {
      output->intent_code = NIFTI_INTENT_VECTOR;
    }
    output->nu
      = output->dim[ 5 ] = ItkImageProperties< ItkImageType >::GetNumberOfComponents();
    if( ItkImageType::ImageDimension  < 4 )
    {
      output->nt
        = output->dim[ 4 ] = 1;
    }
    if( ItkImageType::ImageDimension  < 3 )
    {
      output->nz
        = output->dim[ 3 ] = 1;
    }
    if( ItkImageType::ImageDimension  < 2 )
    {
      output->ny
        = output->dim[ 2 ] = 1;
    }
    if( ItkImageType::ImageDimension  < 1 )
    {
      output->nx
        = output->dim[ 1 ] = 1;
    }
    // Update nvox value because in nifti, vector components are the slowest
    // changing direction, not the fastest.
    output->nvox *= ItkImageProperties< ItkImageType >::GetNumberOfComponents();
  }
  else
  {
    output->ndim     = ItkImageType::ImageDimension;
    output->dim[ 0 ] = ItkImageType::ImageDimension;
  }

  //     -----------------------------------------------------
  //     datatype      needed to specify type of image data
  //     -----------------------------------------------------
  //     bitpix        should correspond correctly to datatype
  //     -----------------------------------------------------
  switch( ItkImageProperties< ItkImageType >::GetComponentType() )
  {
    case IOComponentType::UCHAR:
      output->datatype = NIFTI_TYPE_UINT8;
      output->nbyper   = 1;
      break;
    case IOComponentType::CHAR:
      output->datatype = NIFTI_TYPE_INT8;
      output->nbyper   = 1;
      break;
    case IOComponentType::USHORT:
      output->datatype = NIFTI_TYPE_UINT16;
      output->nbyper   = 2;
      break;
    case IOComponentType::SHORT:
      output->datatype = NIFTI_TYPE_INT16;
      output->nbyper   = 2;
      break;
    case IOComponentType::ULONG:
    case IOComponentType::UINT:
      output->datatype = NIFTI_TYPE_UINT32;
      output->nbyper   = 4;
      break;
    case IOComponentType::LONG:
    case IOComponentType::INT:
      output->datatype = NIFTI_TYPE_INT32;
      output->nbyper   = 4;
      break;
    case IOComponentType::FLOAT:
      output->datatype = NIFTI_TYPE_FLOAT32;
      output->nbyper   = 4;
      break;
    case IOComponentType::DOUBLE:
      output->datatype = NIFTI_TYPE_FLOAT64;
      output->nbyper   = 8;
      break;
    case IOComponentType::UNKNOWNCOMPONENTTYPE:
    default:
    {
      itkGenericExceptionMacro(
          << "More than one component per pixel not supported" );
    }
  }
  switch( ItkImageProperties< ItkImageType >::GetPixelType() )
  {
    case IOPixelType::VECTOR: //NOTE: VECTOR is un-rolled by nifti to look like a
    // multi-dimensional scalar image
    case IOPixelType::SCALAR:
      break;
    case IOPixelType::RGB:
      output->nbyper  *= 3;
      output->datatype = NIFTI_TYPE_RGB24;
      break;
    case IOPixelType::RGBA:
      output->nbyper  *= 4;
      output->datatype = NIFTI_TYPE_RGBA32;
      break;
    case IOPixelType::COMPLEX:
      output->nbyper *= 2;
      switch( ItkImageProperties< ItkImageType >::GetComponentType() )
      {
        case IOComponentType::FLOAT:
          output->datatype = NIFTI_TYPE_COMPLEX64;
          break;
        case IOComponentType::DOUBLE:
          output->datatype = NIFTI_TYPE_COMPLEX128;
          break;
        default:
        {
          itkGenericExceptionMacro(
              << "Only float or double precision complex type supported" );
        }
      }
      break;
    case IOPixelType::SYMMETRICSECONDRANKTENSOR:
    case IOPixelType::DIFFUSIONTENSOR3D:
      break;
    case IOPixelType::OFFSET:
    case IOPixelType::POINT:
    case IOPixelType::COVARIANTVECTOR:
    case IOPixelType::FIXEDARRAY:
    case IOPixelType::MATRIX:
    case IOPixelType::UNKNOWNPIXELTYPE:
    default:
      itkGenericExceptionMacro(
        << "Can not process this pixel type for writing into nifti" );
  }
  //     -----------------------------------------------------
  //     vox_offset    required for an "n+1" header
  //     -----------------------------------------------------
  //     magic         must be "ni1\0" or "n+1\0"
  //     -----------------------------------------------------
  output->scl_slope = 1.0f;
  output->scl_inter = 0.0f;
  //TODO: Note both arguments are the same, no need to distinguish between them.
  SetNIfTIOrientationFromImageIO( input, output, ItkImageType::ImageDimension, ItkImageType::ImageDimension  );
}


template< class ItkImageType, class NiftiPixelType >
void
ItkToNiftiImage< ItkImageType, NiftiPixelType >::SetNIfTIOrientationFromImageIO( typename ItkImageType::Pointer input,
  nifti_image * output,
  unsigned short int origdims,
  unsigned short int dims )
{
  //
  // use NIFTI method 2
  output->sform_code = NIFTI_XFORM_SCANNER_ANAT;
  output->qform_code = NIFTI_XFORM_ALIGNED_ANAT;

  //
  // set the quarternions, from the direction vectors
  //Initialize to size 3 with values of 0
  //
  //The type here must be float, because that matches the signature
  //of the nifti_make_orthog_mat44() method below.
  typedef float DirectionMatrixComponentType;
  int                                         mindims( dims < 3 ? 3 : dims );
  std::vector< DirectionMatrixComponentType > dirx( mindims, 0 );
  unsigned int                                i;
  for( i = 0; i < ItkImageProperties< ItkImageType >::GetDirection( input, 0 ).size(); i++ )
  {
    // KM dirx[ i ] = static_cast< DirectionMatrixComponentType >( -ItkImageProperties< ItkImageType >::GetDirection( input, 0 )[ i ] );
    dirx[ i ] = static_cast< DirectionMatrixComponentType >( ItkImageProperties< ItkImageType >::GetDirection( input, 0 )[ i ] );
  }
  if( i < 3 )
  {
    dirx[ 2 ] = 0.0f;
  }
  std::vector< DirectionMatrixComponentType > diry( mindims, 0 );
  if( origdims > 1 )
  {
    for( i = 0; i < ItkImageProperties< ItkImageType >::GetDirection( input, 1 ).size(); i++ )
    {
      // KM diry[ i ] = static_cast< DirectionMatrixComponentType >( -ItkImageProperties< ItkImageType >::GetDirection( input, 1 )[ i ] );
      diry[ i ] = static_cast< DirectionMatrixComponentType >( ItkImageProperties< ItkImageType >::GetDirection( input, 1 )[ i ] );
    }
    if( i < 3 )
    {
      diry[ 2 ] = 0.0f;
    }
  }
  std::vector< DirectionMatrixComponentType > dirz( mindims, 0 );
  if( origdims > 2 )
  {
    for( unsigned int ii = 0; ii < ItkImageProperties< ItkImageType >::GetDirection( input, 2 ).size(); ii++ )
    {
      // KM dirz[ ii ] = static_cast< DirectionMatrixComponentType >( -ItkImageProperties< ItkImageType >::GetDirection( input, 2 )[ ii ] );
      dirz[ ii ] = static_cast< DirectionMatrixComponentType >( ItkImageProperties< ItkImageType >::GetDirection( input, 2 )[ ii ] );
    }
    //  Read comments in nifti1.h about interpreting
    //  "DICOM Image Orientation (Patient)"
    // KM
    // dirx[ 2 ] = -dirx[ 2 ];
    // diry[ 2 ] = -diry[ 2 ];
    // dirz[ 2 ] = -dirz[ 2 ];
  }
  else
  {
    dirz[ 0 ] = dirz[ 1 ] = 0.0f;
    dirz[ 2 ] = 1.0f;
  }
  mat44 matrix
    = nifti_make_orthog_mat44( dirx[ 0 ], dirx[ 1 ], dirx[ 2 ],
    diry[ 0 ], diry[ 1 ], diry[ 2 ],
    dirz[ 0 ], dirz[ 1 ], dirz[ 2 ] );
  matrix = mat44_transpose( matrix );
  // Fill in origin.
  // KM matrix.m[ 0 ][ 3 ] = static_cast< float >( -ItkImageProperties< ItkImageType >::GetOrigin( input, 0 ) );
  // KM matrix.m[ 1 ][ 3 ] = ( origdims > 1 ) ? static_cast< float >( -ItkImageProperties< ItkImageType >::GetOrigin( input, 1 ) ) : 0.0f;
  matrix.m[ 0 ][ 3 ] = static_cast< float >( ItkImageProperties< ItkImageType >::GetOrigin( input, 0 ) );
  matrix.m[ 1 ][ 3 ] = ( origdims > 1 ) ? static_cast< float >( ItkImageProperties< ItkImageType >::GetOrigin( input, 1 ) ) : 0.0f;
  //NOTE:  The final dimension is not negated!
  matrix.m[ 2 ][ 3 ] = ( origdims > 2 ) ? static_cast< float >( ItkImageProperties< ItkImageType >::GetOrigin( input, 2 ) ) : 0.0f;

  nifti_mat44_to_quatern( matrix,
    &( output->quatern_b ),
    &( output->quatern_c ),
    &( output->quatern_d ),
    &( output->qoffset_x ),
    &( output->qoffset_y ),
    &( output->qoffset_z ),
    ITK_NULLPTR,
    ITK_NULLPTR,
    ITK_NULLPTR,
    &( output->qfac ) );
  // copy q matrix to s matrix
  output->qto_xyz =  matrix;
  output->sto_xyz =  matrix;
  //
  //
  unsigned int sto_limit = origdims > 3 ? 3 : origdims;
  for( unsigned int ii = 0; ii < sto_limit; ii++ )
  {
    for( unsigned int jj = 0; jj < sto_limit; jj++ )
    {
      output->sto_xyz.m[ ii ][ jj ]
        = static_cast< float >( ItkImageProperties< ItkImageType >::GetSpacing( input, jj ) )
        * output->sto_xyz.m[ ii ][ jj ];
    }
  }

  // KM: Force q matrix to have spacing
  output->qto_xyz = output->sto_xyz;

  output->sto_ijk
    = nifti_mat44_inverse( output->sto_xyz );
  output->qto_ijk
    = nifti_mat44_inverse( output->qto_xyz );

  output->pixdim[ 0 ] = output->qfac;
  //  output->sform_code = 0;
}


template< class ItkImageType, class NiftiPixelType >
bool
ItkToNiftiImage< ItkImageType, NiftiPixelType >
::TransferImageData( typename ItkImageType::PixelType * buffer, nifti_image * output )
{
  // Adapted from void NiftiImageIO::Write(const void *buffer)

  unsigned int numComponents = ItkImageProperties< ItkImageType >::GetNumberOfComponents();
  if( numComponents == 1
    || ( numComponents == 2 && ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::COMPLEX )
    || ( numComponents == 3 && ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::RGB )
    || ( numComponents == 4 && ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::RGBA ) )
  {
    // Need a const cast here so that we don't have to copy the memory
    // for writing.
    output->data = (void *)buffer;
    //nifti_image_write(output);
    //output->data = ITK_NULLPTR; // if left pointing to data buffer
    // nifti_image_free will try and free this memory
    return false;
  }
  else  ///Image intent is vector image
  {
    for( unsigned int i = 1; i < 8; i++ )
    {
      if( output->dim[ i ] == 0 )
      {
        output->dim[ i ] = 1;
      }
    }
    const size_t numVoxels
      = size_t( output->dim[ 1 ] )
      * size_t( output->dim[ 2 ] )
      * size_t( output->dim[ 3 ] )
      * size_t( output->dim[ 4 ] );
    const size_t buffer_size
      = numVoxels
      * numComponents //Number of componenets
      * output->nbyper;

    char *             nifti_buf = new char[ buffer_size ];
    const char * const itkbuf    = (const char *)buffer;
    // Data must be rearranged to meet nifti organzation.
    // nifti_layout[vec][t][z][y][x] = itk_layout[t][z][y][z][vec]
    const size_t rowdist    = output->dim[ 1 ];
    const size_t slicedist  = rowdist * output->dim[ 2 ];
    const size_t volumedist = slicedist * output->dim[ 3 ];
    const size_t seriesdist = volumedist * output->dim[ 4 ];
    //
    // as per ITK bug 0007485
    // NIfTI is lower triangular, ITK is upper triangular.
    // i.e. if a symmetric matrix is
    // a b c
    // b d e
    // c e f
    // ITK stores it a b c d e f, but NIfTI is a b d c e f
    // so on read, step sequentially through the source vector, but
    // reverse the order of vec[2] and vec[3]
    int * vecOrder;
    if( ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::DIFFUSIONTENSOR3D
      || ItkImageProperties< ItkImageType >::GetPixelType() == IOPixelType::SYMMETRICSECONDRANKTENSOR )
    {
      vecOrder = UpperToLowerOrder( SymMatDim( numComponents ) );
    }
    else
    {
      vecOrder = new int[ numComponents ];
      for( unsigned i = 0; i < numComponents; i++ )
      {
        vecOrder[ i ] = i;
      }
    }
    for( int t = 0; t < output->dim[ 4 ]; t++ )
    {
      for( int z = 0; z < output->dim[ 3 ]; z++ )
      {
        for( int y = 0; y < output->dim[ 2 ]; y++ )
        {
          for( int x = 0; x < output->dim[ 1 ]; x++ )
          {
            for( unsigned int c = 0; c < numComponents; c++ )
            {
              const size_t nifti_index
                = ( c * seriesdist + volumedist * t + slicedist * z + rowdist * y + x ) * output->nbyper;
              const size_t itk_index
                = ( ( volumedist * t + slicedist * z + rowdist * y
                + x ) * numComponents + vecOrder[ c ] ) * output->nbyper;

              for( int b = 0; b < output->nbyper; ++b )
              {
                nifti_buf[ nifti_index + b ] =  itkbuf[ itk_index + b ];
              }
            }
          }
        }
      }
    }
    delete[] vecOrder;
    //dumpdata(buffer);
    //Need a const cast here so that we don't have to copy the memory for
    //writing.
    output->data = (void *)nifti_buf;
    //nifti_image_write(output);
    //output->data = ITK_NULLPTR; // if left pointing to data buffer
    //delete[] nifti_buf;
    return true;
  }
}
} // end namespace itk
