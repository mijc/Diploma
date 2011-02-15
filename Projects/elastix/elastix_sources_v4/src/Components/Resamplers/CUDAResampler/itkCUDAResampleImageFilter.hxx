/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/
#ifndef __itkCUDAResamplerImageFilter_txx
#define __itkCUDAResamplerImageFilter_txx

#include <cuda_runtime.h>
#include "itkCUDAResampleImageFilter.h"

namespace itk
{

/**
 * ******************* Constructor ***********************
 */

template <typename TInputImage, typename TOutputImage, typename TInterpolatorPrecisionType>
itkCUDAResampleImageFilter<TInputImage, TOutputImage, TInterpolatorPrecisionType>
::itkCUDAResampleImageFilter()
{
  m_UseCuda = true;
  m_UseGPUToCastData = false;

} // end Constructor


/**
 * ******************* Destructor ***********************
 */

template <typename TInputImage, typename TOutputImage, typename TInterpolatorPrecisionType>
itkCUDAResampleImageFilter<TInputImage, TOutputImage, TInterpolatorPrecisionType>
::~itkCUDAResampleImageFilter()
{
  if ( m_UseCuda ) m_CudaResampleImageFilter.cudaUnInit();
}


/**
 * ******************* CopyParameters ***********************
 */

template <typename TInputImage, typename TOutputImage, typename TInterpolatorPrecisionType>
void
itkCUDAResampleImageFilter<TInputImage, TOutputImage, TInterpolatorPrecisionType>
::CopyParameters( typename InternalAdvancedBSplineTransformType::Pointer bSplineTransform )
{
  /* Copy parameters to the GPU memory space. */
  const SizeType        itkOutputSize    = this->GetSize();
  const SizeType        itkInputSize     = this->GetInput()->GetLargestPossibleRegion().GetSize();
  const SpacingType     itkOutputSpacing = this->GetOutputSpacing();
  const OriginPointType itkOutputOrigin  = this->GetOutputOrigin();
  const SpacingType     itkInputSpacing  = this->GetInput()->GetSpacing();
  const OriginPointType itkInputOrigin   = this->GetInput()->GetOrigin();

  int3 inputsize             = make_int3( itkInputSize[0],  itkInputSize[1],  itkInputSize[2] );
  int3 outputsize            = make_int3( itkOutputSize[0], itkOutputSize[1], itkOutputSize[2] );
  const InputPixelType* data = this->GetInput()->GetBufferPointer();
  m_CudaResampleImageFilter.cudaMallocImageData( inputsize, outputsize, data );

  float3 outputimageSpacing = make_float3( itkOutputSpacing[0], itkOutputSpacing[1], itkOutputSpacing[2] );
  float3 outputimageOrigin  = make_float3( itkOutputOrigin[0],  itkOutputOrigin[1],  itkOutputOrigin[2] );
  float3 inputimageSpacing  = make_float3( itkInputSpacing[0],  itkInputSpacing[1],  itkInputSpacing[2] );
  float3 inputimageOrigin   = make_float3( itkInputOrigin[0],   itkInputOrigin[1],   itkInputOrigin[2] );

  float defaultPixelValue   = this->GetDefaultPixelValue();
  m_CudaResampleImageFilter.cudaCopyImageSymbols( inputimageSpacing, inputimageOrigin,
    outputimageSpacing, outputimageOrigin, defaultPixelValue );

  const typename InternalBSplineTransformType::OriginType  ITKgridOrigin
    = bSplineTransform->GetGridOrigin();
  const typename InternalBSplineTransformType::SpacingType ITKgridSpacing
    = bSplineTransform->GetGridSpacing();
  const typename InternalBSplineTransformType::SizeType    ITKgridSize
    = bSplineTransform->GetGridRegion().GetSize();

  float3 gridSpacing        = make_float3( ITKgridSpacing[0],   ITKgridSpacing[1],   ITKgridSpacing[2] );
  float3 gridOrigin         = make_float3( ITKgridOrigin[0],    ITKgridOrigin[1],    ITKgridOrigin[2] );
  int3   gridSize           = make_int3  ( ITKgridSize[0],      ITKgridSize[1],      ITKgridSize[2] );
  m_CudaResampleImageFilter.cudaCopyGridSymbols( gridSpacing, gridOrigin, gridSize );

  const typename InternalBSplineTransformType::ParametersType params
    = bSplineTransform->GetParameters();

  m_CudaResampleImageFilter.cudaMallocTransformationData( gridSize, params.data_block() );

} // end CopyParameters()


/**
 * ******************* CheckForValidTransform ***********************
 */

template <typename TInputImage, typename TOutputImage, typename TInterpolatorPrecisionType>
bool
itkCUDAResampleImageFilter<TInputImage, TOutputImage, TInterpolatorPrecisionType>
::CheckForValidTransform( ValidTransformPointer & bSplineTransform ) const
{
  /** First check if the Transform is valid for CUDA. */
  typename InternalBSplineTransformType::Pointer testPtr1a
    = const_cast<InternalBSplineTransformType *>(
    dynamic_cast<const InternalBSplineTransformType *>( this->GetTransform() ) );
  typename InternalAdvancedBSplineTransformType::Pointer testPtr1b
    = const_cast<InternalAdvancedBSplineTransformType *>(
    dynamic_cast<const InternalAdvancedBSplineTransformType *>( this->GetTransform() ) );
  typename InternalComboTransformType::Pointer testPtr2a
    = const_cast<InternalComboTransformType *>(
    dynamic_cast<const InternalComboTransformType *>( this->GetTransform() ) );

  bool transformIsValid = false;
  if ( testPtr1a )
  {
    /** The transform is of type BSplineDeformableTransform. */
    //transformIsValid = true;
    transformIsValid = false; // \todo: not yet supported
    //bSplineTransform = testPtr1a;
  }
  else if ( testPtr1b )
  {
    /** The transform is of type AdvancedBSplineDeformableTransform. */
    transformIsValid = true;
    bSplineTransform = testPtr1b;
  }
  else if ( testPtr2a )
  {
    // Check that the comboT has no initial transform and that current = B-spline
    // and that B-spline = 3rd order

    /** The transform is of type AdvancedCombinationTransform. */
    if ( !testPtr2a->GetInitialTransform() )
    {
      typename InternalAdvancedBSplineTransformType::Pointer testPtr2b
        = dynamic_cast<InternalAdvancedBSplineTransformType *>(
        testPtr2a->GetCurrentTransform() );
      if ( testPtr2b )
      {
        /** The current transform is of type AdvancedBSplineDeformableTransform. */
        transformIsValid = true;
        bSplineTransform = testPtr2b;
      }
    }
  } // end if combo transform

  return transformIsValid;

} // end CheckForValidTransform()


/**
 * ******************* GenerateData ***********************
 */

template <typename TInputImage, typename TOutputImage, typename TInterpolatorPrecisionType>
void
itkCUDAResampleImageFilter<TInputImage, TOutputImage, TInterpolatorPrecisionType>
::GenerateData( void )
{
  /** If we are not using CUDA simply use the CPU implementation. */
  if ( !m_UseCuda )
  {
    return Superclass::GenerateData();
  }

  /** Check! */
  ValidTransformPointer tempTransform = NULL;
  try // why try/catch?
  {
    /** Check for valid transform. */
    bool transformIsValid = this->CheckForValidTransform( tempTransform );
    if ( !transformIsValid )
    {
      itkWarningMacro( << "WARNING: Using CPU (no B-spline transform set)" );
    }

    /** Check if proper CUDA device. */
    bool cuda_device = ( CudaResampleImageFilterType::checkExecutionParameters() == 0 );
    if ( !cuda_device )
    {
      itkWarningMacro( << "WARNING: Using CPU (no CUDA capable GPU found, and/or up-to-date driver)" );
    }

    m_UseCuda = transformIsValid && cuda_device;
  }
  catch ( itk::ExceptionObject & excep )
  {
    // FIXME: no printing
    std::cerr << excep << std::endl;
    m_UseCuda = false;
  }

  /** The GPU can't be used. Use CPU instead. */
  if ( !m_UseCuda )
  {
    return Superclass::GenerateData();
  }

  /** Initialize CUDA device. */
  m_CudaResampleImageFilter.cudaInit();
  m_CudaResampleImageFilter.SetCastOnGPU( m_UseGPUToCastData );

  /** Copy the parameters to the GPU. */
  this->CopyParameters( tempTransform );

  /** Allocate host memory for the output and copy/cast the result back to the host. */
  this->AllocateOutputs();
  InputPixelType* data = this->GetOutput()->GetBufferPointer();

  /** Run the CUDA resampler. */
  m_CudaResampleImageFilter.GenerateData( data );

} // end GenerateData()


}; /* namespace itk */

#endif // end #ifndef __itkCUDAResamplerImageFilter_txx

