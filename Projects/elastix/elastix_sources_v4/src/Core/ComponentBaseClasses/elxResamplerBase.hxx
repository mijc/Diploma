/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __elxResamplerBase_hxx
#define __elxResamplerBase_hxx

#include "elxResamplerBase.h"
#include "itkImageFileCastWriter.h"
#include "itkChangeInformationImageFilter.h"
#include "elxTimer.h"

namespace elastix
{
using namespace itk;

/*
 * ******************* BeforeRegistrationBase *******************
 */

template<class TElastix>
void
ResamplerBase<TElastix>
::BeforeRegistrationBase( void )
{
  /** Connect the components. */
  this->SetComponents();

  /** Set the size of the image to be produced by the resampler. */

  /** Get a pointer to the fixedImage.
   * \todo make it a cast to the fixed image type
   */
  typedef typename ElastixType::FixedImageType FixedImageType;
  FixedImageType * fixedImage = this->m_Elastix->GetFixedImage();

  /** Set the region info to the same values as in the fixedImage. */
  this->GetAsITKBaseType()->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
  this->GetAsITKBaseType()->SetOutputStartIndex( fixedImage->GetLargestPossibleRegion().GetIndex() );
  this->GetAsITKBaseType()->SetOutputOrigin( fixedImage->GetOrigin() );
  this->GetAsITKBaseType()->SetOutputSpacing( fixedImage->GetSpacing() );
  this->GetAsITKBaseType()->SetOutputDirection( fixedImage->GetDirection() );

  /** Set the DefaultPixelValue (for pixels in the resampled image
   * that come from outside the original (moving) image.
   */
  double defaultPixelValue = NumericTraits<double>::Zero;
  bool found = this->m_Configuration->ReadParameter(
    defaultPixelValue, "DefaultPixelValue", 0, false );

  /** Set the defaultPixelValue. int values overrule double values. */
  if ( found )
  {
    this->GetAsITKBaseType()->SetDefaultPixelValue(
      static_cast<OutputPixelType>( defaultPixelValue ) );
  }

} // end BeforeRegistrationBase()


/*
 * ******************* AfterEachResolutionBase ********************
 */

template<class TElastix>
void
ResamplerBase<TElastix>
::AfterEachResolutionBase( void )
{
  /** Set the final transform parameters. */
  this->GetElastix()->GetElxTransformBase()->SetFinalParameters();

  /** What is the current resolution level? */
  const unsigned int level = this->m_Registration->GetAsITKBaseType()->GetCurrentLevel();

  /** Decide whether or not to write the result image this resolution. */
  bool writeResultImageThisResolution = false;
  this->m_Configuration->ReadParameter( writeResultImageThisResolution,
    "WriteResultImageAfterEachResolution", "", level, 0, false );

  /** Writing result image. */
  if ( writeResultImageThisResolution )
  {
    /** Create a name for the final result. */
    std::string resultImageFormat = "mhd";
    this->m_Configuration->ReadParameter( resultImageFormat,
      "ResultImageFormat", 0, false );
    std::ostringstream makeFileName( "" );
    makeFileName
      << this->m_Configuration->GetCommandLineArgument( "-out" )
      << "result." << this->m_Configuration->GetElastixLevel()
      << ".R" << level
      << "." << resultImageFormat;

    /** Time the resampling. */
    typedef tmr::Timer TimerType;
    TimerType::Pointer timer = TimerType::New();
    timer->StartTimer();

    /** Apply the final transform, and save the result. */
    elxout << "Applying transform this resolution ..." << std::endl;
    try
    {
      this->WriteResultImage( makeFileName.str().c_str() );
    }
    catch( itk::ExceptionObject & excp )
    {
      xl::xout["error"] << "Exception caught: " << std::endl;
      xl::xout["error"] << excp
        << "Resuming elastix." << std::endl;
    }

    /** Print the elapsed time for the resampling. */
    timer->StopTimer();
    elxout << "  Applying transform took "
      << static_cast<long>( timer->GetElapsedClockSec() )
      << " s." << std::endl;

  } // end if

} // end AfterEachResolutionBase()


/*
 * ******************* AfterRegistrationBase ********************
 */

template<class TElastix>
void
ResamplerBase<TElastix>
::AfterRegistrationBase( void )
{
  /** Set the final transform parameters. */
  this->GetElastix()->GetElxTransformBase()->SetFinalParameters();

  /** Decide whether or not to write the result image. */
  std::string writeResultImage = "true";
  this->m_Configuration->ReadParameter(
    writeResultImage, "WriteResultImage", 0 );

  /** Release memory to be able to resample in case a limited
   * amount of memory is available.
   */
  this->ReleaseMemory();

  /** Writing result image. */
  if ( writeResultImage == "true" )
  {
    /** Create a name for the final result. */
    std::string resultImageFormat = "mhd";
    this->m_Configuration->ReadParameter(
      resultImageFormat, "ResultImageFormat", 0);
    std::ostringstream makeFileName( "" );
    makeFileName
      << this->m_Configuration->GetCommandLineArgument( "-out" )
      << "result." << this->m_Configuration->GetElastixLevel()
      << "." << resultImageFormat;

    /** Time the resampling. */
    typedef tmr::Timer TimerType;
    TimerType::Pointer timer = TimerType::New();
    timer->StartTimer();

    /** Apply the final transform, and save the result,
     * by calling WriteResultImage.
     */
    elxout << "\nApplying final transform ..." << std::endl;
    try
    {
      this->WriteResultImage( makeFileName.str().c_str() );
    }
    catch( itk::ExceptionObject & excp )
    {
      xl::xout["error"] << "Exception caught: " << std::endl;
      xl::xout["error"] << excp
        << "Resuming elastix." << std::endl;
    }

    /** Print the elapsed time for the resampling. */
    timer->StopTimer();
    elxout << "  Applying final transform took "
      << static_cast<long>( timer->GetElapsedClockSec() )
      << " s." << std::endl;
  }
  else
  {
    /** Do not apply the final transform. */
    elxout << std::endl
      << "Skipping applying final transform, no resulting output image generated."
      << std::endl;
  } // end if

} // end AfterRegistrationBase()


/*
* *********************** SetComponents ************************
*/

template <class TElastix>
void
ResamplerBase<TElastix>
::SetComponents( void )
{
  /** Set the transform, the interpolator and the inputImage
   * (which is the moving image).
   */
  this->GetAsITKBaseType()->SetTransform( dynamic_cast<TransformType *>(
    this->m_Elastix->GetElxTransformBase() ) );

  this->GetAsITKBaseType()->SetInterpolator( dynamic_cast<InterpolatorType *>(
    this->m_Elastix->GetElxResampleInterpolatorBase() ) );

  this->GetAsITKBaseType()->SetInput( dynamic_cast<InputImageType *>(
    this->m_Elastix->GetMovingImage() ) );

} // end SetComponents()


/*
 * ******************* WriteResultImage ********************
 */

template<class TElastix>
void
ResamplerBase<TElastix>
::WriteResultImage( const char * filename )
{
  /** Make sure the resampler is updated. */
  this->GetAsITKBaseType()->Modified();

  /** Add a progress observer to the resampler. */
  typename ProgressCommandType::Pointer progressObserver = ProgressCommandType::New();
  progressObserver->ConnectObserver( this->GetAsITKBaseType() );
  progressObserver->SetStartString( "  Progress: " );
  progressObserver->SetEndString( "%" );

  /** Do the resampling. */
  try
  {
    this->GetAsITKBaseType()->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    /** Add information to the exception. */
    excp.SetLocation( "ResamplerBase - WriteResultImage()" );
    std::string err_str = excp.GetDescription();
    err_str += "\nError occurred while resampling the image.\n";
    excp.SetDescription( err_str );

    /** Pass the exception to an higher level. */
    throw excp;
  }

  /** Read output pixeltype from parameter the file. Replace possible " " with "_". */
  std::string resultImagePixelType = "short";
  this->m_Configuration->ReadParameter( resultImagePixelType,
    "ResultImagePixelType", 0, false );
  std::basic_string<char>::size_type pos = resultImagePixelType.find( " " );
  const std::basic_string<char>::size_type npos = std::basic_string<char>::npos;
  if ( pos != npos ) resultImagePixelType.replace( pos, 1, "_" );

  /** Read from the parameter file if compression is desired. */
  bool doCompression = false;
  this->m_Configuration->ReadParameter(
    doCompression, "CompressResultImage", 0, false );

  /** Typedef's for writing the output image. */
  typedef ImageFileCastWriter< OutputImageType >  WriterType;
  typedef typename WriterType::Pointer            WriterPointer;
  typedef ChangeInformationImageFilter<
    OutputImageType >                             ChangeInfoFilterType;

  /** Possibly change direction cosines to their original value, as specified
   * in the tp-file, or by the fixed image. This is only necessary when
   * the UseDirectionCosines flag was set to false.
   */
  typename ChangeInfoFilterType::Pointer infoChanger = ChangeInfoFilterType::New();
  DirectionType originalDirection;
  bool retdc = this->GetElastix()->GetOriginalFixedImageDirection( originalDirection );
  infoChanger->SetOutputDirection( originalDirection );
  infoChanger->SetChangeDirection( retdc & !this->GetElastix()->GetUseDirectionCosines() );
  infoChanger->SetInput( this->GetAsITKBaseType()->GetOutput() );

  /** Create writer. */
  WriterPointer writer = WriterType::New();

  /** Setup the pipeline. */
  writer->SetInput( infoChanger->GetOutput() );
  writer->SetFileName( filename );
  writer->SetOutputComponentType( resultImagePixelType.c_str() );
  writer->SetUseCompression( doCompression );

  /** Do the writing. */
  xl::xout["coutonly"] << std::flush;
  xl::xout["coutonly"] << "\n  Writing image ..." << std::endl;
  try
  {
    writer->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    /** Add information to the exception. */
    excp.SetLocation( "ResamplerBase - AfterRegistrationBase()" );
    std::string err_str = excp.GetDescription();
    err_str += "\nError occurred while writing resampled image.\n";
    excp.SetDescription( err_str );

    /** Pass the exception to an higher level. */
    throw excp;
  }

  /** Disconnect from the resampler. */
  progressObserver->DisconnectObserver( this->GetAsITKBaseType() );

} // end WriteResultImage()


/*
 * ************************* ReadFromFile ***********************
 */

template<class TElastix>
void
ResamplerBase<TElastix>
::ReadFromFile( void )
{
  /** Connect the components. */
  this->SetComponents();

  /** Get spacing, origin and size of the image to be produced by the resampler. */
  SpacingType     spacing;
  IndexType       index;
  OriginPointType origin;
  SizeType        size;
  DirectionType   direction;
  direction.SetIdentity();
  for ( unsigned int i = 0; i < ImageDimension; i++ )
  {
    /** No default size. Read size from the parameter file. */
    this->m_Configuration->ReadParameter( size[ i ], "Size", i );

    /** Default index. Read index from the parameter file. */
    index[ i ] = 0;
    this->m_Configuration->ReadParameter( index[ i ], "Index", i );

    /** Default spacing. Read spacing from the parameter file. */
    spacing[ i ] = 1.0;
    this->m_Configuration->ReadParameter( spacing[ i ], "Spacing", i );

    /** Default origin. Read origin from the parameter file. */
    origin[ i ] = 0.0;
    this->m_Configuration->ReadParameter( origin[ i ], "Origin", i );

    /** Read direction cosines. Default identity */
    for ( unsigned int j = 0; j < ImageDimension; j++ )
    {
      this->m_Configuration->ReadParameter( direction( j, i ),
        "Direction", i * ImageDimension + j );
    }
  }

  /** Check for image size. */
  unsigned int sum = 0;
  for ( unsigned int i = 0; i < ImageDimension; i++ )
  {
    if ( size[ i ] == 0 ) sum++;
  }
  if ( sum > 0 )
  {
    xl::xout["error"] << "ERROR: One or more image sizes are 0!" << std::endl;
    /** \todo quit program nicely. */
  }

  /** Set the region info to the same values as in the fixedImage. */
  this->GetAsITKBaseType()->SetSize( size );
  this->GetAsITKBaseType()->SetOutputStartIndex( index );
  this->GetAsITKBaseType()->SetOutputOrigin( origin );
  this->GetAsITKBaseType()->SetOutputSpacing( spacing );

  /** Set the direction cosines. If no direction cosines
   * should be used, set identity cosines, to simulate the
   * old ITK behavior.
   */
  if ( ! this->GetElastix()->GetUseDirectionCosines() )
  {
    direction.SetIdentity();
  }
  this->GetAsITKBaseType()->SetOutputDirection( direction );

  /** Set the DefaultPixelValue (for pixels in the resampled image
   * that come from outside the original (moving) image.
   */
  double defaultPixelValue = NumericTraits<double>::Zero;
  bool found = this->m_Configuration->ReadParameter( defaultPixelValue,
    "DefaultPixelValue", 0, false );

  if ( found )
  {
    this->GetAsITKBaseType()->SetDefaultPixelValue(
      static_cast<OutputPixelType>( defaultPixelValue ) );
  }

} // end ReadFromFile()


/**
 * ******************* WriteToFile ******************************
 */

template <class TElastix>
void
ResamplerBase<TElastix>
::WriteToFile( void ) const
{
  /** Write resampler specific things. */
  xl::xout["transpar"] << std::endl << "// Resampler specific" << std::endl;

  /** Write the name of the resampler. */
  xl::xout["transpar"] << "(Resampler \""
    << this->elxGetClassName() << "\")" << std::endl;

  /** Write the DefaultPixelValue. */
  xl::xout["transpar"] << "(DefaultPixelValue "
    << this->GetAsITKBaseType()->GetDefaultPixelValue() << ")" << std::endl;

  /** Write the output image format. */
  std::string resultImageFormat = "mhd";
  this->m_Configuration->ReadParameter(
    resultImageFormat, "ResultImageFormat", 0, false );
  xl::xout["transpar"] << "(ResultImageFormat \""
    << resultImageFormat << "\")" << std::endl;

  /** Write output pixel type. */
  std::string resultImagePixelType = "short";
  this->m_Configuration->ReadParameter(
    resultImagePixelType, "ResultImagePixelType", 0, false );
  xl::xout["transpar"] << "(ResultImagePixelType \""
    << resultImagePixelType << "\")" << std::endl;

  /** Write compression flag. */
  std::string doCompression = "false";
  this->m_Configuration->ReadParameter(
    doCompression, "CompressResultImage", 0, false );
  xl::xout["transpar"] << "(CompressResultImage \""
    << doCompression << "\")" << std::endl;

} // end WriteToFile()


/*
 * ******************* ReleaseMemory ********************
 */

template<class TElastix>
void ResamplerBase<TElastix>
::ReleaseMemory( void )
{
  /** Release some memory. Sometimes it is not possible to
   * resample and write an image, because too much memory is consumed by
   * elastix. Releasing some memory at this point helps a lot.
   */

  /** Release more memory, but only if this is the final elastix level. */
  if ( this->GetConfiguration()->GetElastixLevel() + 1
    == this->GetConfiguration()->GetTotalNumberOfElastixLevels() )
  {
    /** Release fixed image memory. */
    const unsigned int nofi = this->GetElastix()->GetNumberOfFixedImages();
    for ( unsigned int i = 0; i < nofi; ++i )
    {
      this->GetElastix()->GetFixedImage( i )->ReleaseData();
    }

    /** Release fixed mask image memory. */
    const unsigned int nofm = this->GetElastix()->GetNumberOfFixedMasks();
    for ( unsigned int i = 0; i < nofm; ++i )
    {
      if ( this->GetElastix()->GetFixedMask( i ) != 0 )
      {
        this->GetElastix()->GetFixedMask( i )->ReleaseData();
      }
    }

    /** Release moving mask image memory. */
    const unsigned int nomm = this->GetElastix()->GetNumberOfMovingMasks();
    for ( unsigned int i = 0; i < nomm; ++i )
    {
      if ( this->GetElastix()->GetMovingMask( i ) != 0 )
      {
        this->GetElastix()->GetMovingMask( i )->ReleaseData();
      }
    }

  } // end if final elastix level

  /** The B-spline interpolator stores a coefficient image of doubles the
   * size of the moving image. We clear it by setting the input image to
   * zero. The interpolator is not needed anymore, since we have the
   * resampler interpolator.
   */
  this->GetElastix()->GetElxInterpolatorBase()->GetAsITKBaseType()->SetInputImage( 0 );

  // Clear ImageSampler, metric, optimizer, interpolator, registration, internal images?

} // end ReleaseMemory()


} // end namespace elastix


#endif // end #ifndef __elxResamplerBase_hxx

