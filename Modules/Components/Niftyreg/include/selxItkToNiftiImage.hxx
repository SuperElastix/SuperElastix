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
//#include "itkIOCommon.h"
//#include "itkMetaDataObject.h"
//#include "itkSpatialOrientationAdapter.h"

namespace selx
{

// returns an ordering array for converting upper triangular symmetric matrix
// to lower triangular symmetric matrix
int *
UpperToLowerOrder(int dim)
{
  int **mat = new int *[dim];

  for ( int i = 0; i < dim; i++ )
    {
    mat[i] = new int[dim];
    }
  // fill in
  int index(0);
  for ( int i = 0; i < dim; i++ )
    {
    for ( int j = i; j < dim; j++ )
      {
      mat[i][j] = index;
      mat[j][i] = index;
      index++;
      }
    }
  int *rval = new int[index + 1];
  int  index2(0);
  for ( int i = 0; i < dim; i++ )
    {
    for ( int j = 0; j <= i; j++, index2++ )
      {
      rval[index2] = mat[i][j];
      }
    }
  rval[index2] = -1;
  for ( int i = 0; i < dim; i++ )
    {
    delete[] mat[i];
    }
  delete[] mat;
  return rval;
}

// returns an ordering array for converting lower triangular symmetric matrix
// to upper triangular symmetric matrix
int *
LowerToUpperOrder(int dim)
{
  int **mat = new int *[dim];

  for ( int i = 0; i < dim; i++ )
    {
    mat[i] = new int[dim];
    }
  // fill in
  int index(0);
  for ( int i = 0; i < dim; i++ )
    {
    for ( int j = 0; j <= i; j++, index++ )
      {
      mat[i][j] = index;
      mat[j][i] = index;
      }
    }
  int *rval = new int[index + 1];
  int  index2(0);
  for ( int i = 0; i < dim; i++ )
    {
    for ( int j = i; j < dim; j++, index2++ )
      {
      rval[index2] = mat[i][j];
      }
    }
  rval[index2] = -1;
  for ( int i = 0; i < dim; i++ )
    {
    delete[] mat[i];
    }
  delete[] mat;
  return rval;
}

// compute the rank of the symmetric matrix from
// the count of the triangular matrix elements
int SymMatDim(int count)
{
  int dim = 0;
  int row = 1;

  while ( count > 0 )
    {
    count -= row;
    dim++;
    row++;
    }
  return dim;
}


ItkToNiftiImage::ItkToNiftiImage():
  m_NiftiImage(ITK_NULLPTR),
  m_RescaleSlope(1.0),
  m_RescaleIntercept(0.0),
  m_NumberOfDimensions(3)
{
  nifti_set_debug_level(0); // suppress error messages
}

ItkToNiftiImage::~ItkToNiftiImage()
{
  nifti_image_free(this->m_NiftiImage);
}


bool
ItkToNiftiImage::MustRescale()
{
  return std::abs(this->m_RescaleSlope) > std::numeric_limits< double >::epsilon()
         && ( std::abs(this->m_RescaleSlope - 1.0) > std::numeric_limits< double >::epsilon()
              || std::abs(this->m_RescaleIntercept) > std::numeric_limits< double >::epsilon() );
}

// Internal function to rescale pixel according to Rescale Slope/Intercept
template< typename TBuffer >
void RescaleFunction(TBuffer *buffer,
                     double slope,
                     double intercept,
                     size_t size)
{
  for ( size_t i = 0; i < size; i++ )
    {
    double tmp = static_cast< double >( buffer[i] ) * slope;
    tmp += intercept;
    buffer[i] = static_cast< TBuffer >( tmp );
    }
}

template< typename PixelType >
void
CastCopy(float *to, void *from, size_t pixelcount)
{
  PixelType *_from = static_cast< PixelType * >( from );

  for ( unsigned i = 0; i < pixelcount; i++ )
    {
    to[i] = static_cast< float >( _from[i] );
    }
}


namespace
{
inline mat44 mat44_transpose(mat44 in)
{
  mat44 out;

  for ( unsigned int i = 0; i < 4; i++ )
    {
    for ( unsigned int j = 0; j < 4; j++ )
      {
      out.m[i][j] = in.m[j][i];
      }
    }
  return out;
}
}
void
ItkToNiftiImage
::WriteImageInformation(void)
{
  //  MetaDataDictionary &thisDic=this->GetMetaDataDictionary();
  //
  //
  // First of all we need to not go any further if there's
  // a dimension of the image that won't fit in a 16 bit short.
  for ( unsigned int i = 0; i < this->m_NumberOfDimensions; i++ )
    {
    unsigned int curdim( this->GetDimensions(i) );
    if ( curdim > static_cast< unsigned int >( NumericTraits< short >::max() ) )
      {
      itkExceptionMacro( << "Dimension(" << i << ") = " << curdim
                         << " is greater than maximum possible dimension "
                         << NumericTraits< short >::max() );
      }
    }

  // fill out the image header.
  if ( this->m_NiftiImage == ITK_NULLPTR )
    {
    this->m_NiftiImage = nifti_simple_init_nim();
    }
  //
  // set the filename
  //std::string FName( "selxItkToNiftiImage.nii" );
  const std::string BaseName("selxItkToNiftiImage"); 
  const bool                   IsCompressed  = false
  this->m_NiftiImage->nifti_type = NIFTI_FTYPE_NIFTI1_1;
 
  this->m_NiftiImage->fname = nifti_makehdrname(BaseName.c_str(), this->m_NiftiImage->nifti_type, false, IsCompressed);
  this->m_NiftiImage->iname = nifti_makeimgname(BaseName.c_str(), this->m_NiftiImage->nifti_type, false, IsCompressed);
  //     FIELD         NOTES
  //     -----------------------------------------------------
  //     sizeof_hdr    must be 348
  //     -----------------------------------------------------
  //     dim           dim[0] and dim[1] are always required;
  //                   dim[2] is required for 2-D volumes,
  //                   dim[3] for 3-D volumes, etc.
  this->m_NiftiImage->nvox = 1;
  //Spacial dims in ITK are given in mm.
  //If 4D assume 4thD is in SECONDS, for all of ITK.
  //NOTE: Due to an ambiguity in the nifti specification, some developers
  // external tools believe that the time units must be set, even if there
  // is only one dataset.  Having the time specified for a purly spatial
  // image has no consequence, so go ahead and set it to seconds.
  this->m_NiftiImage->xyz_units = static_cast< int >( NIFTI_UNITS_MM | NIFTI_UNITS_SEC );
  this->m_NiftiImage->dim[7] = this->m_NiftiImage->nw = 1;
  this->m_NiftiImage->dim[6] = this->m_NiftiImage->nv = 1;
  this->m_NiftiImage->dim[5] = this->m_NiftiImage->nu = 1;
  this->m_NiftiImage->dim[4] = this->m_NiftiImage->nt = 1;
  this->m_NiftiImage->dim[3] = this->m_NiftiImage->nz = 1;
  this->m_NiftiImage->dim[2] = this->m_NiftiImage->ny = 1;
  this->m_NiftiImage->dim[1] = this->m_NiftiImage->nx = 1;
  switch ( this->m_NumberOfDimensions )
    {
    case 7:
      this->m_NiftiImage->dim[7] = this->m_NiftiImage->nw =
                                     static_cast< int >( this->GetDimensions(6) );
      this->m_NiftiImage->pixdim[7] = this->m_NiftiImage->dw =
                                        static_cast< float >( this->GetSpacing(6) );
      this->m_NiftiImage->nvox *= this->m_NiftiImage->dim[7];
      ITK_FALLTHROUGH;
    case 6:
      this->m_NiftiImage->dim[6] = this->m_NiftiImage->nv =
                                     this->GetDimensions(5);
      this->m_NiftiImage->pixdim[6] = this->m_NiftiImage->dv =
                                        static_cast< float >( this->GetSpacing(5) );
      this->m_NiftiImage->nvox *= this->m_NiftiImage->dim[6];
      ITK_FALLTHROUGH;
    case 5:
      this->m_NiftiImage->dim[5] =
        this->m_NiftiImage->nu = this->GetDimensions(4);
      this->m_NiftiImage->pixdim[5] =
        this->m_NiftiImage->du = static_cast< float >( this->GetSpacing(4) );
      this->m_NiftiImage->nvox *= this->m_NiftiImage->dim[5];
      ITK_FALLTHROUGH;
    case 4:
      this->m_NiftiImage->dim[4] = this->m_NiftiImage->nt =
                                     this->GetDimensions(3);
      this->m_NiftiImage->pixdim[4] =
        this->m_NiftiImage->dt = static_cast< float >( this->GetSpacing(3) );
      this->m_NiftiImage->nvox *= this->m_NiftiImage->dim[4];
      ITK_FALLTHROUGH;
    case 3:
      this->m_NiftiImage->dim[3] = this->m_NiftiImage->nz =
                                     this->GetDimensions(2);
      this->m_NiftiImage->pixdim[3] =
        this->m_NiftiImage->dz = static_cast< float >( this->GetSpacing(2) );
      this->m_NiftiImage->nvox *= this->m_NiftiImage->dim[3];
      ITK_FALLTHROUGH;
    case 2:
      this->m_NiftiImage->dim[2] = this->m_NiftiImage->ny =
                                     this->GetDimensions(1);
      this->m_NiftiImage->pixdim[2] =
        this->m_NiftiImage->dy = static_cast< float >( this->GetSpacing(1) );
      this->m_NiftiImage->nvox *= this->m_NiftiImage->dim[2];
      ITK_FALLTHROUGH;
    case 1:
      this->m_NiftiImage->dim[1] = this->m_NiftiImage->nx =
                                     this->GetDimensions(0);
      this->m_NiftiImage->pixdim[1] =
        this->m_NiftiImage->dx = static_cast< float >( this->GetSpacing(0) );
      this->m_NiftiImage->nvox *= this->m_NiftiImage->dim[1];
    }

  const unsigned int numComponents = this->GetNumberOfComponents();

  //TODO:  Also need to check for RGB images where numComponets=3
  if ( numComponents > 1
       && !( this->GetPixelType() == COMPLEX
             &&  numComponents == 2 )
       && !( this->GetPixelType() == RGB
             && numComponents == 3 )
       && !( this->GetPixelType() == RGBA
             && numComponents == 4 ) )
    {
    this->m_NiftiImage->ndim = 5;   //This must be 5 for NIFTI_INTENT_VECTOR
                                    // images.
    this->m_NiftiImage->dim[0] = 5; //This must be 5 for NIFTI_INTENT_VECTOR
                                    // images.
    if ( this->m_NumberOfDimensions > 4 )
      {
      itkExceptionMacro(
         << "Can not store a vector image of more than 4 dimensions in a Nifti file. Dimension="
         << this->m_NumberOfDimensions );
      }
    //
    // support symmetric matrix type
    if ( this->GetPixelType() == ImageIOBase::DIFFUSIONTENSOR3D
         || this->GetPixelType() == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
      {
      this->m_NiftiImage->intent_code = NIFTI_INTENT_SYMMATRIX;
      }
    else
      {
      this->m_NiftiImage->intent_code = NIFTI_INTENT_VECTOR;
      }
    this->m_NiftiImage->nu =
      this->m_NiftiImage->dim[5] = this->GetNumberOfComponents();
    if ( this->m_NumberOfDimensions < 4 )
      {
      this->m_NiftiImage->nt =
        this->m_NiftiImage->dim[4] = 1;
      }
    if ( this->m_NumberOfDimensions < 3 )
      {
      this->m_NiftiImage->nz =
        this->m_NiftiImage->dim[3] = 1;
      }
    if ( this->m_NumberOfDimensions < 2 )
      {
      this->m_NiftiImage->ny =
        this->m_NiftiImage->dim[2] = 1;
      }
    if ( this->m_NumberOfDimensions < 1 )
      {
      this->m_NiftiImage->nx =
        this->m_NiftiImage->dim[1] = 1;
      }
    // Update nvox value because in nifti, vector components are the slowest
    // changing direction, not the fastest.
    this->m_NiftiImage->nvox *= this->GetNumberOfComponents();
    }
  else
    {
    this->m_NiftiImage->ndim = this->m_NumberOfDimensions;
    this->m_NiftiImage->dim[0] = this->m_NumberOfDimensions;
    }

  //     -----------------------------------------------------
  //     datatype      needed to specify type of image data
  //     -----------------------------------------------------
  //     bitpix        should correspond correctly to datatype
  //     -----------------------------------------------------
  switch ( this->GetComponentType() )
    {
    case UCHAR:
      this->m_NiftiImage->datatype = NIFTI_TYPE_UINT8;
      this->m_NiftiImage->nbyper = 1;
      break;
    case CHAR:
      this->m_NiftiImage->datatype = NIFTI_TYPE_INT8;
      this->m_NiftiImage->nbyper = 1;
      break;
    case USHORT:
      this->m_NiftiImage->datatype = NIFTI_TYPE_UINT16;
      this->m_NiftiImage->nbyper = 2;
      break;
    case SHORT:
      this->m_NiftiImage->datatype = NIFTI_TYPE_INT16;
      this->m_NiftiImage->nbyper = 2;
      break;
    case ULONG:
    case UINT:
      this->m_NiftiImage->datatype = NIFTI_TYPE_UINT32;
      this->m_NiftiImage->nbyper = 4;
      break;
    case LONG:
    case INT:
      this->m_NiftiImage->datatype = NIFTI_TYPE_INT32;
      this->m_NiftiImage->nbyper = 4;
      break;
    case FLOAT:
      this->m_NiftiImage->datatype = NIFTI_TYPE_FLOAT32;
      this->m_NiftiImage->nbyper = 4;
      break;
    case DOUBLE:
      this->m_NiftiImage->datatype = NIFTI_TYPE_FLOAT64;
      this->m_NiftiImage->nbyper = 8;
      break;
    case UNKNOWNCOMPONENTTYPE:
    default:
      {
      itkExceptionMacro(
        << "More than one component per pixel not supported");
      }
    }
  switch ( this->GetPixelType() )
    {
    case VECTOR: //NOTE: VECTOR is un-rolled by nifti to look like a
                 // multi-dimensional scalar image
    case SCALAR:
      break;
    case RGB:
      this->m_NiftiImage->nbyper *= 3;
      this->m_NiftiImage->datatype = NIFTI_TYPE_RGB24;
      break;
    case RGBA:
      this->m_NiftiImage->nbyper *= 4;
      this->m_NiftiImage->datatype = NIFTI_TYPE_RGBA32;
      break;
    case COMPLEX:
      this->m_NiftiImage->nbyper *= 2;
      switch ( this->GetComponentType() )
        {
        case FLOAT:
          this->m_NiftiImage->datatype = NIFTI_TYPE_COMPLEX64;
          break;
        case DOUBLE:
          this->m_NiftiImage->datatype = NIFTI_TYPE_COMPLEX128;
          break;
        default:
          {
          itkExceptionMacro(
            << "Only float or double precision complex type supported");
          }
        }
      break;
    case SYMMETRICSECONDRANKTENSOR:
    case DIFFUSIONTENSOR3D:
      break;
    case OFFSET:
    case POINT:
    case COVARIANTVECTOR:
    case FIXEDARRAY:
    case MATRIX:
    case UNKNOWNPIXELTYPE:
    default:
      itkExceptionMacro(
        << "Can not process this pixel type for writing into nifti");
    }
  //     -----------------------------------------------------
  //     vox_offset    required for an "n+1" header
  //     -----------------------------------------------------
  //     magic         must be "ni1\0" or "n+1\0"
  //     -----------------------------------------------------
  this->m_NiftiImage->scl_slope = 1.0f;
  this->m_NiftiImage->scl_inter = 0.0f;
  //TODO: Note both arguments are the same, no need to distinguish between them.
  this->SetNIfTIOrientationFromImageIO( this->m_NumberOfDimensions, this->m_NumberOfDimensions );
}


void
ItkToNiftiImage::SetNIfTIOrientationFromImageIO(unsigned short int origdims, unsigned short int dims)
{
  //
  // use NIFTI method 2
  this->m_NiftiImage->sform_code = NIFTI_XFORM_SCANNER_ANAT;
  this->m_NiftiImage->qform_code = NIFTI_XFORM_ALIGNED_ANAT;

  //
  // set the quarternions, from the direction vectors
  //Initialize to size 3 with values of 0
  //
  //The type here must be float, because that matches the signature
  //of the nifti_make_orthog_mat44() method below.
  typedef float DirectionMatrixComponentType;
  int                                         mindims(dims < 3 ? 3 : dims);
  std::vector< DirectionMatrixComponentType > dirx(mindims, 0);
  unsigned int                                i;
  for ( i = 0; i < this->GetDirection(0).size(); i++ )
    {
    dirx[i] = static_cast< DirectionMatrixComponentType >( -this->GetDirection(0)[i] );
    }
  if ( i < 3 )
    {
    dirx[2] = 0.0f;
    }
  std::vector< DirectionMatrixComponentType > diry(mindims, 0);
  if ( origdims > 1 )
    {
    for ( i = 0; i < this->GetDirection(1).size(); i++ )
      {
      diry[i] = static_cast< DirectionMatrixComponentType >( -this->GetDirection(1)[i] );
      }
    if ( i < 3 )
      {
      diry[2] = 0.0f;
      }
    }
  std::vector< DirectionMatrixComponentType > dirz(mindims, 0);
  if ( origdims > 2 )
    {
    for ( unsigned int ii = 0; ii < this->GetDirection(2).size(); ii++ )
      {
      dirz[ii] = static_cast< DirectionMatrixComponentType >( -this->GetDirection(2)[ii] );
      }
    //  Read comments in nifti1.h about interpreting
    //  "DICOM Image Orientation (Patient)"
    dirx[2] = -dirx[2];
    diry[2] = -diry[2];
    dirz[2] = -dirz[2];
    }
  else
    {
    dirz[0] = dirz[1] = 0.0f;
    dirz[2] = 1.0f;
    }
  mat44 matrix =
    nifti_make_orthog_mat44(dirx[0], dirx[1], dirx[2],
                            diry[0], diry[1], diry[2],
                            dirz[0], dirz[1], dirz[2]);
  matrix = mat44_transpose(matrix);
  // Fill in origin.
  matrix.m[0][3] =  static_cast< float >( -this->GetOrigin(0) );
  matrix.m[1][3] = ( origdims > 1 ) ? static_cast< float >( -this->GetOrigin(1) ) : 0.0f;
  //NOTE:  The final dimension is not negated!
  matrix.m[2][3] = ( origdims > 2 ) ? static_cast< float >( this->GetOrigin(2) ) : 0.0f;

  nifti_mat44_to_quatern( matrix,
                          &( this->m_NiftiImage->quatern_b ),
                          &( this->m_NiftiImage->quatern_c ),
                          &( this->m_NiftiImage->quatern_d ),
                          &( this->m_NiftiImage->qoffset_x ),
                          &( this->m_NiftiImage->qoffset_y ),
                          &( this->m_NiftiImage->qoffset_z ),
                          ITK_NULLPTR,
                          ITK_NULLPTR,
                          ITK_NULLPTR,
                          &( this->m_NiftiImage->qfac ) );
  // copy q matrix to s matrix
  this->m_NiftiImage->qto_xyz =  matrix;
  this->m_NiftiImage->sto_xyz =  matrix;
  //
  //
  unsigned int sto_limit = origdims > 3 ? 3 : origdims;
  for ( unsigned int ii = 0; ii < sto_limit; ii++ )
    {
    for ( unsigned int jj = 0; jj < sto_limit; jj++ )
      {
      this->m_NiftiImage->sto_xyz.m[ii][jj] =
        static_cast< float >( this->GetSpacing(jj) )
        * this->m_NiftiImage->sto_xyz.m[ii][jj];
      }
    }
  this->m_NiftiImage->sto_ijk =
    nifti_mat44_inverse(this->m_NiftiImage->sto_xyz);
  this->m_NiftiImage->qto_ijk =
    nifti_mat44_inverse(this->m_NiftiImage->qto_xyz);

  this->m_NiftiImage->pixdim[0] = this->m_NiftiImage->qfac;
  //  this->m_NiftiImage->sform_code = 0;
}

void
ItkToNiftiImage
::Write(const void *buffer)
{
  // Write the image Information before writing data
  this->WriteImageInformation();
  unsigned int numComponents = this->GetNumberOfComponents();
  if ( numComponents == 1
       || ( numComponents == 2 && this->GetPixelType() == COMPLEX )
       || ( numComponents == 3 && this->GetPixelType() == RGB )
       || ( numComponents == 4 && this->GetPixelType() == RGBA ) )
    {
    // Need a const cast here so that we don't have to copy the memory
    // for writing.
    this->m_NiftiImage->data = const_cast< void * >( buffer );
    nifti_image_write(this->m_NiftiImage);
    this->m_NiftiImage->data = ITK_NULLPTR; // if left pointing to data buffer
    // nifti_image_free will try and free this memory
    }
  else  ///Image intent is vector image
    {
    for ( unsigned int i = 1; i < 8; i++ )
      {
      if ( this->m_NiftiImage->dim[i] == 0 )
        {
        this->m_NiftiImage->dim[i] = 1;
        }
      }
    const size_t numVoxels =
      size_t(this->m_NiftiImage->dim[1])
      * size_t(this->m_NiftiImage->dim[2])
      * size_t(this->m_NiftiImage->dim[3])
      * size_t(this->m_NiftiImage->dim[4]);
    const size_t buffer_size =
      numVoxels
      * numComponents //Number of componenets
      * this->m_NiftiImage->nbyper;

    char *            nifti_buf = new char[buffer_size];
    const char *const itkbuf = (const char *)buffer;
    // Data must be rearranged to meet nifti organzation.
    // nifti_layout[vec][t][z][y][x] = itk_layout[t][z][y][z][vec]
    const size_t rowdist = m_NiftiImage->dim[1];
    const size_t slicedist = rowdist * m_NiftiImage->dim[2];
    const size_t volumedist = slicedist * m_NiftiImage->dim[3];
    const size_t seriesdist = volumedist * m_NiftiImage->dim[4];
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
    int *vecOrder;
    if ( this->GetPixelType() == ImageIOBase::DIFFUSIONTENSOR3D
         || this->GetPixelType() == ImageIOBase::SYMMETRICSECONDRANKTENSOR )
      {
      vecOrder = UpperToLowerOrder( SymMatDim(numComponents) );
      }
    else
      {
      vecOrder = new int[numComponents];
      for ( unsigned i = 0; i < numComponents; i++ )
        {
        vecOrder[i] = i;
        }
      }
    for ( int t = 0; t < this->m_NiftiImage->dim[4]; t++ )
      {
      for ( int z = 0; z < this->m_NiftiImage->dim[3]; z++ )
        {
        for ( int y = 0; y < this->m_NiftiImage->dim[2]; y++ )
          {
          for ( int x = 0; x < this->m_NiftiImage->dim[1]; x++ )
            {
            for ( unsigned int c = 0; c < numComponents; c++ )
              {
              const size_t nifti_index =
                ( c * seriesdist + volumedist * t + slicedist * z + rowdist * y + x ) * this->m_NiftiImage->nbyper;
              const size_t itk_index =
                ( ( volumedist * t + slicedist * z + rowdist * y
                    + x ) * numComponents + vecOrder[c] ) * this->m_NiftiImage->nbyper;

              for( int b = 0; b < this->m_NiftiImage->nbyper; ++b )
                {
                nifti_buf[nifti_index+b] =  itkbuf[itk_index+b];
                }
              }
            }
          }
        }
      }
    delete[] vecOrder;
    dumpdata(buffer);
    //Need a const cast here so that we don't have to copy the memory for
    //writing.
    this->m_NiftiImage->data = (void *)nifti_buf;
    nifti_image_write(this->m_NiftiImage);
    this->m_NiftiImage->data = ITK_NULLPTR; // if left pointing to data buffer
    delete[] nifti_buf;
    }
}
} // end namespace itk
