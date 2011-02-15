/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef _itkCombinationImageToImageMetric_txx
#define _itkCombinationImageToImageMetric_txx

#include "itkCombinationImageToImageMetric.h"


/** Macros to reduce some copy-paste work.
 * These macros provide the implementation of
 * all Set/GetFixedImage, Set/GetInterpolator etc methods
 *
 * The macros are undef'ed at the end of this file
 */

/** For setting objects, implement two methods */
#define itkImplementationSetObjectMacro2(_name, _type1, _type2 ) \
template <class TFixedImage, class TMovingImage> \
void \
CombinationImageToImageMetric<TFixedImage,TMovingImage> \
::Set##_name ( _type1 _type2 *_arg, unsigned int pos ) \
{ \
  if ( pos == 0 ) \
  { \
    this->Superclass::Set##_name ( _arg ); \
  } \
  ImageMetricType    * testPtr1 \
    = dynamic_cast<ImageMetricType *>( this->GetMetric( pos ) ); \
  PointSetMetricType * testPtr2 \
    = dynamic_cast<PointSetMetricType *>( this->GetMetric( pos ) ); \
  if ( testPtr1 ) \
  { \
    testPtr1->Set##_name ( _arg ); \
  } \
  else if ( testPtr2 ) \
  { \
    testPtr2->Set##_name ( _arg ); \
  } \
} \
template <class TFixedImage, class TMovingImage> \
void \
CombinationImageToImageMetric<TFixedImage,TMovingImage> \
::Set##_name ( _type1 _type2 *_arg ) \
{ \
  for ( unsigned int i = 0; i < this->GetNumberOfMetrics(); i++ ) \
  { \
    this->Set##_name ( _arg, i ); \
  } \
}  // comments for allowing ; after calling the macro

#define itkImplementationSetObjectMacro1(_name, _type1, _type2 ) \
template <class TFixedImage, class TMovingImage> \
void \
CombinationImageToImageMetric<TFixedImage,TMovingImage> \
::Set##_name ( _type1 _type2 *_arg, unsigned int pos ) \
{ \
  if ( pos == 0 ) \
  { \
    this->Superclass::Set##_name ( _arg ); \
  } \
  ImageMetricType    * testPtr1 \
    = dynamic_cast<ImageMetricType *>( this->GetMetric( pos ) ); \
  if ( testPtr1 ) \
  { \
    testPtr1->Set##_name ( _arg ); \
  } \
} \
template <class TFixedImage, class TMovingImage> \
void \
CombinationImageToImageMetric<TFixedImage,TMovingImage> \
::Set##_name ( _type1 _type2 *_arg ) \
{ \
  for ( unsigned int i = 0; i < this->GetNumberOfMetrics(); i++ ) \
  { \
    this->Set##_name ( _arg, i ); \
  } \
}  // comments for allowing ; after calling the macro

/** for getting const object, implement one method */
#define itkImplementationGetConstObjectMacro1(_name,_type) \
template <class TFixedImage, class TMovingImage> \
const typename CombinationImageToImageMetric<TFixedImage,TMovingImage>:: \
  _type * CombinationImageToImageMetric<TFixedImage,TMovingImage> \
::Get##_name ( unsigned int pos ) const \
{ \
  const ImageMetricType    * testPtr1 \
    = dynamic_cast<const ImageMetricType * >( this->GetMetric( pos ) ); \
  if ( testPtr1 ) \
  { \
    return testPtr1->Get##_name (); \
  } \
  else \
  { \
    return 0; \
  } \
}  // comments for allowing ; after calling the macro

#define itkImplementationGetConstObjectMacro2(_name,_type) \
template <class TFixedImage, class TMovingImage> \
const typename CombinationImageToImageMetric<TFixedImage,TMovingImage>:: \
  _type * CombinationImageToImageMetric<TFixedImage,TMovingImage> \
::Get##_name ( unsigned int pos ) const \
{ \
  const ImageMetricType    * testPtr1 \
    = dynamic_cast<const ImageMetricType * >( this->GetMetric( pos ) ); \
  const PointSetMetricType * testPtr2 \
    = dynamic_cast<const PointSetMetricType * >( this->GetMetric( pos ) ); \
  if ( testPtr1 ) \
  { \
    return testPtr1->Get##_name (); \
  } \
  else if ( testPtr2 ) \
  { \
    return testPtr2->Get##_name (); \
  } \
  else \
  { \
    return 0; \
  } \
}  // comments for allowing ; after calling the macro


namespace itk
{

itkImplementationSetObjectMacro2( Transform, , TransformType );
itkImplementationSetObjectMacro1( Interpolator, , InterpolatorType );
itkImplementationSetObjectMacro2( FixedImageMask, , FixedImageMaskType );
itkImplementationSetObjectMacro2( MovingImageMask, , MovingImageMaskType );
itkImplementationSetObjectMacro1( FixedImage, const, FixedImageType );
itkImplementationSetObjectMacro1( MovingImage, const, MovingImageType );

itkImplementationGetConstObjectMacro2( Transform, TransformType );
itkImplementationGetConstObjectMacro1( Interpolator, InterpolatorType );
itkImplementationGetConstObjectMacro2( FixedImageMask, FixedImageMaskType );
itkImplementationGetConstObjectMacro2( MovingImageMask, MovingImageMaskType );
itkImplementationGetConstObjectMacro1( FixedImage, FixedImageType );
itkImplementationGetConstObjectMacro1( MovingImage, MovingImageType );


/**
 * ********************* Constructor ****************************
 */

template <class TFixedImage, class TMovingImage>
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::CombinationImageToImageMetric()
{
  this->m_NumberOfMetrics = 0;
  this->ComputeGradientOff();

} // end Constructor


/**
 * ********************* PrintSelf ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  /** Call the superclass' PrintSelf. */
  Superclass::PrintSelf( os, indent );

  /** Add debugging information. */
  os << indent << "NumberOfMetrics: "
    << this->m_NumberOfMetrics << std::endl;
  os << indent << "Metric pointer, weight, value:" << std::endl;
  for ( unsigned int i = 0; i < this->m_NumberOfMetrics; i++ )
  {
    os << indent << "Metric " << i << ": "
      << this->m_Metrics[ i ].GetPointer() << ", "
      << this->m_MetricWeights[ i ] << ", "
      << this->m_MetricValues[ i ] << ", ";
    if ( this->m_UseMetric[ i ] )
    {
      os << "used" << std::endl;
    }
    else
    {
      os << "not used" << std::endl;
    }
  }

} // end PrintSelf()


/**
 * ******************** SetFixedImageRegion ************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::SetFixedImageRegion( const FixedImageRegionType _arg, unsigned int pos )
{
  if ( pos == 0 )
  {
    this->Superclass::SetFixedImageRegion( _arg );
  }
  ImageMetricType * testPtr = dynamic_cast<ImageMetricType *>(
    this->GetMetric( pos ) );
  if ( testPtr )
  {
    testPtr->SetFixedImageRegion( _arg );
  }

} // end SetFixedImageRegion()


/**
 * ******************** SetFixedImageRegion ************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::SetFixedImageRegion( const FixedImageRegionType _arg )
{
  for ( unsigned int i = 0; i < this->GetNumberOfMetrics(); i++ )
  {
    this->SetFixedImageRegion( _arg, i );
  }

} // end SetFixedImageRegion()


/**
 * ******************** GetFixedImageRegion ************************
 */

template <class TFixedImage, class TMovingImage>
const typename CombinationImageToImageMetric<TFixedImage,TMovingImage>
::FixedImageRegionType &
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetFixedImageRegion( unsigned int pos ) const
{
  const ImageMetricType * testPtr
    = dynamic_cast<const ImageMetricType *>( this->GetMetric( pos ) );
  if ( testPtr )
  {
    return testPtr->GetFixedImageRegion();
  }
  else
  {
    return this->m_NullFixedImageRegion;
  }

} // end GetFixedImageRegion()


/**
 * ********************* SetNumberOfMetrics ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::SetNumberOfMetrics( unsigned int count )
{
  if ( count != this->m_Metrics.size() )
  {
    this->m_NumberOfMetrics = count;
    this->m_Metrics.resize( count );
    this->m_MetricWeights.resize( count );
    this->m_UseMetric.resize( count );
    this->m_MetricValues.resize( count );
    this->m_MetricDerivatives.resize( count );
    this->Modified();
  }

} // end SetNumberOfMetrics()


/**
 * ********************* SetMetric ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::SetMetric( SingleValuedCostFunctionType * metric, unsigned int pos )
{
  if ( pos >= this->GetNumberOfMetrics() )
  {
    this->SetNumberOfMetrics( pos + 1 );
  }

  if ( metric != this->m_Metrics[ pos ] )
  {
    this->m_Metrics[ pos ] = metric;
    this->Modified();
  }

} // end SetMetric()


/**
 * ********************* GetMetric ****************************
 */

template <class TFixedImage, class TMovingImage>
typename CombinationImageToImageMetric<TFixedImage,TMovingImage>
::SingleValuedCostFunctionType *
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetMetric( unsigned int pos ) const
{
  if ( pos >= this->GetNumberOfMetrics() )
  {
    return 0;
  }
  else
  {
    return this->m_Metrics[ pos ];
  }

} // end GetMetric()


/**
 * ********************* SetMetricWeight ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::SetMetricWeight( double weight, unsigned int pos )
{
  if ( pos >= this->GetNumberOfMetrics() )
  {
    this->SetNumberOfMetrics( pos + 1 );
  }

  if ( weight != this->m_MetricWeights[ pos ] )
  {
    this->m_MetricWeights[ pos ] = weight;
    this->Modified();
  }

} // end SetMetricsWeight()


/**
 * ********************* GetMetricWeight ****************************
 */

template <class TFixedImage, class TMovingImage>
double
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetMetricWeight( unsigned int pos ) const
{
  if ( pos >= this->GetNumberOfMetrics() )
  {
    return 0.0;
  }
  else
  {
    return this->m_MetricWeights[ pos ];
  }

} // end GetMetricWeight()


/**
 * ********************* SetUseMetric ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::SetUseMetric( const bool use, const unsigned int pos )
{
  if ( pos >= this->GetNumberOfMetrics() )
  {
    this->SetNumberOfMetrics( pos + 1 );
  }

  if ( use != this->m_UseMetric[ pos ] )
  {
    this->m_UseMetric[ pos ] = use;
    this->Modified();
  }

} // end SetUseMetric()


/**
 * ********************* SetUseAllMetrics ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::SetUseAllMetrics( void )
{
  for ( unsigned int pos = 0; pos < this->GetNumberOfMetrics(); ++pos )
  {
    if ( !this->m_UseMetric[ pos ] )
    {
      this->m_UseMetric[ pos ] = true;
      this->Modified();
    }
  }

} // end SetUseAllMetrics()


/**
 * ********************* GetUseMetric ****************************
 */

template <class TFixedImage, class TMovingImage>
bool
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetUseMetric( unsigned int pos ) const
{
  if ( pos >= this->GetNumberOfMetrics() )
  {
    return false;
  }
  else
  {
    return this->m_UseMetric[ pos ];
  }

} // end GetUseMetric()


/**
 * ********************* GetMetricValue ****************************
 */

template <class TFixedImage, class TMovingImage>
typename CombinationImageToImageMetric<TFixedImage,TMovingImage>::MeasureType
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetMetricValue( unsigned int pos ) const
{
  if ( pos >= this->GetNumberOfMetrics() )
  {
    return 0.0;
  }
  else
  {
    return this->m_MetricValues[ pos ];
  }

} // end GetMetricValue()


/**
 * ********************* GetMetricDerivative ****************************
 */

template <class TFixedImage, class TMovingImage>
const typename CombinationImageToImageMetric<TFixedImage,TMovingImage>
::DerivativeType &
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetMetricDerivative( unsigned int pos ) const
{
  if ( pos >= this->GetNumberOfMetrics() )
  {
    return this->m_NullDerivative;
  }
  else
  {
    return this->m_MetricDerivatives[ pos ];
  }

} // end GetMetricValue()


/**
 * **************** GetNumberOfPixelsCounted ************************
 */

template <class TFixedImage, class TMovingImage>
const unsigned long &
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetNumberOfPixelsCounted( void ) const
{
  unsigned long sum = 0;
  for ( unsigned int i = 0; i < this->GetNumberOfMetrics(); ++i )
  {
    const ImageMetricType * testPtr
      = dynamic_cast<const ImageMetricType *>( this->GetMetric(i) );
    if ( testPtr )
    {
      sum += testPtr->GetNumberOfPixelsCounted();
    }
  }

  this->m_NumberOfPixelsCounted = sum;
  return this->m_NumberOfPixelsCounted;

} // end GetNumberOfPixelsCounted()


/**
 * ********************* Initialize ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::Initialize( void ) throw ( ExceptionObject )
{
  /** Check if transform, interpolator have been set. Effectively this
   * method checks if the first sub metric is set up completely.
   * This implicitly means that the first sub metric is an ImageToImageMetric,
   * which is a reasonable demand.
   */
  this->Superclass::Initialize();

  /** Check if at least one (image)metric is provided */
  if ( this->GetNumberOfMetrics() == 0 )
  {
    itkExceptionMacro( << "At least one metric should be set!" );
  }

  /** Check the first metric. */
//   ImageMetricType * firstMetric = dynamic_cast<ImageMetricType *>(
//     this->GetMetric( 0 ) );
//   if ( !firstMetric )
//   {
//     itkExceptionMacro( << "The first sub metric must be of type ImageToImageMetric!" );
//     itkWarningMacro( << "The first sub metric is not of type ImageToImageMetric!" );
//   }

  /** Call Initialize for all metrics. */
  for ( unsigned int i = 0; i < this->GetNumberOfMetrics() ; i++ )
  {
    SingleValuedCostFunctionType * costfunc = this->GetMetric( i );
    if ( !costfunc )
    {
      itkExceptionMacro( << "Metric " << i << " has not been set!" );
    }
    ImageMetricType    * testPtr1 = dynamic_cast<ImageMetricType *>( this->GetMetric( i ) );
    PointSetMetricType * testPtr2 = dynamic_cast<PointSetMetricType *>( this->GetMetric( i ) );
    if ( testPtr1 )
    {
      testPtr1->Initialize();
    }
    else if ( testPtr2 )
    {
      testPtr2->Initialize();
    }
  }

} // end Initialize()


/**
 * ********************* GetValue ****************************
 */

template <class TFixedImage, class TMovingImage>
typename CombinationImageToImageMetric<TFixedImage,TMovingImage>::MeasureType
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetValue( const ParametersType & parameters ) const
{
  /** Initialise. */
  MeasureType measure = NumericTraits< MeasureType >::Zero;

  /** Add all metric values. */
  for ( unsigned int i = 0; i < this->m_NumberOfMetrics; i++ )
  {
    MeasureType tmpValue = this->m_Metrics[ i ]->GetValue( parameters );

    if ( this->m_UseMetric[ i ] )
    {
      measure += this->m_MetricWeights[ i ] * tmpValue;
    }

    /** Store for later, for interested users. */
    this->m_MetricValues[ i ] = tmpValue;
  }

  /** Return a value. */
  return measure;

} // end GetValue()


/**
 * ********************* GetDerivative ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetDerivative( const ParametersType & parameters,
  DerivativeType & derivative ) const
{
  /** Initialise. */
  DerivativeType tmpDerivative = DerivativeType( this->GetNumberOfParameters() );
  derivative = DerivativeType( this->GetNumberOfParameters() );
  derivative.Fill( NumericTraits< MeasureType >::Zero );

  /** Add all metric derivatives. */
  for ( unsigned int i = 0; i < this->m_NumberOfMetrics; i++ )
  {
    tmpDerivative.Fill( NumericTraits< MeasureType >::Zero );
    this->m_Metrics[ i ]->GetDerivative( parameters, tmpDerivative );

    if ( this->m_UseMetric[ i ] )
    {
      derivative += this->m_MetricWeights[ i ] * tmpDerivative;
    }

    /** Store for later, for interested users. */
    this->m_MetricDerivatives[ i ] = tmpDerivative;
  }

} // end GetDerivative()


/**
 * ********************* GetValueAndDerivative ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivative(
  const ParametersType & parameters,
  MeasureType & value,
  DerivativeType & derivative ) const
{
  /** Initialise. */
  MeasureType tmpValue = NumericTraits< MeasureType >::Zero;
  value = NumericTraits< MeasureType >::Zero;

  DerivativeType tmpDerivative = DerivativeType( this->GetNumberOfParameters() );
  derivative = DerivativeType( this->GetNumberOfParameters() );
  derivative.Fill( NumericTraits< MeasureType >::Zero );

  /** Add all metric values and derivatives. */
  for ( unsigned int i = 0; i < this->m_NumberOfMetrics; i++ )
  {
    tmpValue = NumericTraits< MeasureType >::Zero;
    tmpDerivative.Fill( NumericTraits< MeasureType >::Zero );
    this->m_Metrics[ i ]->GetValueAndDerivative( parameters, tmpValue, tmpDerivative );

    if ( this->m_UseMetric[ i ] )
    {
      value += this->m_MetricWeights[ i ] * tmpValue;
      derivative += this->m_MetricWeights[ i ] * tmpDerivative;
    }

    /** Store for later, for interested users. */
    this->m_MetricValues[ i ] = tmpValue;
    this->m_MetricDerivatives[ i ] = tmpDerivative;

  }

} // end GetValueAndDerivative()

/**
 * ********************* GetSelfHessian ****************************
 */

template <class TFixedImage, class TMovingImage>
void
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetSelfHessian( const TransformParametersType & parameters,
  HessianType & H ) const
{
  /** Prepare Hessian */
  H.SetSize( this->GetNumberOfParameters(),
    this->GetNumberOfParameters() );
  H.Fill(0.0);
  HessianType tmpH( this->GetNumberOfParameters(),
    this->GetNumberOfParameters() );

  /** Add all metrics' selfhessians. */
  bool initialized = false;
  for ( unsigned int i = 0; i < this->m_NumberOfMetrics; i++ )
  {
    if ( this->m_UseMetric[ i ] )
    {
      ImageMetricType * metric = dynamic_cast<ImageMetricType *>( this->GetMetric( i ) );
      if ( metric )
      {
        initialized = true;
        metric->GetSelfHessian( parameters, tmpH );
        H += this->m_MetricWeights[ i ] * tmpH;
      }
    }
  }

  /** If none of the submetrics has a valid implementation of GetSelfHessian,
   * then return an identity matrix */
  if ( ! initialized )
  {
    H.fill_diagonal(1.0);
  }

} // end GetSelfHessian()


/**
 * ********************* GetMTime ****************************
 */

template <class TFixedImage, class TMovingImage>
unsigned long
CombinationImageToImageMetric<TFixedImage,TMovingImage>
::GetMTime( void ) const
{
  unsigned long mtime = this->Superclass::GetMTime();
  unsigned long m;

  // Some of the following should be removed once this 'ivars' are put in the
  // input and output lists

  /** Check the modified time of the sub metrics */
  for ( unsigned int i = 0; i < this->GetNumberOfMetrics(); ++i )
  {
    SingleValuedCostFunctionPointer metric = this->GetMetric( i );
    if ( metric.IsNotNull() )
    {
      m = metric->GetMTime();
      mtime = ( m > mtime ? m : mtime );
    }
  }

  return mtime;

} // end GetMTime()


} // end namespace itk


#undef itkImplementationSetObjectMacro1
#undef itkImplementationSetObjectMacro2
#undef itkImplementationGetConstObjectMacro1
#undef itkImplementationGetConstObjectMacro2

#endif // end #ifndef _itkCombinationImageToImageMetric_txx

