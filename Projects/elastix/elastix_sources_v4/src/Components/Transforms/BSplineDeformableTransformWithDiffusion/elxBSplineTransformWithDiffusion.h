/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __elxBSplineTransformWithDiffusion_H__
#define __elxBSplineTransformWithDiffusion_H__

/* For easy changing the BSplineOrder: */
#define __VSplineOrder 3

#include "itkAdvancedBSplineDeformableTransform.h"
#include "itkAdvancedCombinationTransform.h"
//#include "itkBSplineCombinationTransform.h"
#include "itkBSplineResampleImageFilterBase.h"
#include "itkBSplineUpsampleImageFilter.h"

#include "itkImageRegionConstIterator.h"

#include "elxIncludes.h"

/** Include structure for the diffusion. */
#include "itkDeformationFieldRegulizer.h"
#include "itkVectorMeanDiffusionImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkMaximumImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkBSplineInterpolateImageFunction.h"


namespace elastix
{
using namespace itk;


  /**
   * \class BSplineTransformWithDiffusion
   * \brief This class combines a B-spline transform with the
   * diffusion/filtering of the deformation field.
   *
   * Every n iterations the deformation field is diffused using the
   * VectorMeanDiffusionImageFilter. The total transformation of a point
   * is determined by adding the B-spline deformation to the
   * deformation field arrow. Filtering of the deformation field is based
   * on some 'stiffness coefficient' image.
   *
   * \todo: this Transform has not been tested for images with Direction cosines
   * matrix other than the identity matrix.
   *
   * \warning Using this transform in with the option
   * (HowToCombineTranforms "Compose"). May give unpredictable results.
   * Especially if the initial transformation is large. This is because
   * the coefficient grid is not properly initialized.
   * Better use (HowToCombineTranforms "Add").
   *
   * The parameters used in this class are:
   * \parameter Transform: Select this transform as follows:\n
   *    <tt>(%Transform "BSplineTransformWithDiffusion")</tt>
   * \parameter FinalGridSpacing: the grid spacing of the B-spline transform
   *    part of this transform for each dimension. \n
   *    example: <tt>(FinalGridSpacing 8.0 8.0 8.0)</tt>
   *    If only one argument is given, that factor is used for each dimension. The spacing
   *    is not in millimeters, but in "voxel size units".
   *    The default is 8.0 in every dimension.
   * \parameter UpsampleGridOption: whether or not the B-spline grid should
   *    be upsampled from one resolution level to another. Choose from {true, false}. \n
   *    example: <tt>(UpsampleGridOption "true")</tt>
   *    The default is "true".
   * \parameter FilterPattern: defines according to what schedule/pattern
   *    the deformation field should be filtered. Choose from {1,2}, where
   *    FilterPattern 1 is diffusion every "DiffusionEachNIterations" iterations,
   *    and where FilterPattern 2 filters more frequent in the beginning
   *    and less frequent at the end of a resolution. \n
   *    example: <tt>(FilterPattern 1)</tt>
   *    The default is filter pattern 1.
   * \parameter DiffusionEachNIterations: defines for FilterPattern 1 after how
   *    many iterations of the optimiser there should be a filtering step. \n
   *    example: <tt>(DiffusionEachNIterations 5)</tt>
   *    The default is 1.
   * \parameter AfterIterations: defines for FilterPattern 2 after how many
   *    iterations of the optimiser the filter frequency should be increased. \n
   *    example: <tt>(AfterIterations 100 200)</tt>
   *    The default is 50 and 100.
   * \parameter HowManyIterations: defines to what frequency the filtering
   *    should be increased. \n
   *    example: <tt>(HowManyIterations 1 5 10)</tt>
   *    The default is 1, 5 and 10.
   * \parameter NumberOfDiffusionIterations: defines the number of times
   *    the adaptive filtering is performed. \n
   *    example: <tt>(NumberOfDiffusionIterations 10)</tt>
   *    The default is 1.
   * \parameter Radius: defines the radius of the filter. \n
   *    example: <tt>(Radius 1)</tt>
   *    The default is 1.
   * \parameter ThresholdBool: defines whether or not the stiffness coefficient
   *    image should be thresholded. Choose from {true, false}. \n
   *    example: <tt>(ThresholdBool "true")</tt>
   *    The default is "true".
   * \parameter ThresholdHU: if it is thresholded, this defines the threshold in
   *    Houndsfield units. \n
   *    example: <tt>(ThresholdHU 150)</tt>
   *    The default is 150.
   * \parameter WriteDiffusionFiles: defines whether or not the stiffness coefficient
   *    image, the deformation field and the filtered field should be written
   *    to file. Choose from {true, false}. \n
   *    example: <tt>(WriteDiffusionFiles "true")</tt>
   *    The default is "false".
   * \parameter GrayValueImageAlsoBasedOnFixedImage: defines whether or not
   *    the stiffness coefficient image should also be based on the fixed image.
   *    Choose from {true, false}. \n
   *    example: <tt>(GrayValueImageAlsoBasedOnFixedImage "true")</tt>
   *    The default is "true".
   * \parameter UseFixedSegmentation: defines whether or not the stiffness coefficient
   *    image should be based on some prior defined segmentation of rigid structures in
   *    the fixed image, instead on a thresholding. Choose from {true, false}. \n
   *    example: <tt>(UseFixedSegmentation "true")</tt>
   *    The default is "false".
   * \parameter FixedSegmentationFileName: the filename of this segmentation. \n
   *    example: <tt>(FixedSegmentationFileName "somestring")</tt>
   * \parameter UseMovingSegmentation: defines whether or not the stiffness coefficient
   *    image should be based on some prior defined segmentation of rigid structures in
   *    the moving image, instead on a thresholding. Choose from {true, false}. \n
   *    example: <tt>(UseMovingSegmentation "true")</tt>
   *    The default is "false".
   * \parameter MovingSegmentationFileName: the filename of this segmentation. \n
   *    example: <tt>(MovingSegmentationFileName "somestring")</tt>
   * \parameter DefaultPixelValueForGVI: the default pixel value, when resampling
   *    the grayvalueimage.
   *    example: <tt>(DefaultPixelValueForGVI 0)</tt>
   *    The default is 0.
   *
   * The transform parameters necessary for transformix, additionally defined by this class, are:
   * \transformparameter DeformationFieldFileName: stores the name of the deformation field. \n
   *    example: <tt>(DeformationFieldFileName "defField.mhd")</tt>
   * \transformparameter GridSize: stores the size of the B-spline grid. \n
   *    example: <tt>(GridSize 16 16 16)</tt>
   * \transformparameter GridIndex: stores the index of the B-spline grid. \n
   *    example: <tt>(GridIndex 0 0 0)</tt>
   * \transformparameter GridSpacing: stores the spacing of the B-spline grid. \n
   *    example: <tt>(GridSpacing 16.0 16.0 16.0)</tt>
   * \transformparameter GridOrigin: stores the origin of the B-spline grid. \n
   *    example: <tt>(GridOrigin 0.0 0.0 0.0)</tt>
   *
   * \ingroup Transforms
   */

template < class TElastix >
class BSplineTransformWithDiffusion
  : public
  DeformationFieldRegulizer<
    AdvancedCombinationTransform<
    //BSplineCombinationTransform<
    ITK_TYPENAME elx::TransformBase<TElastix>::CoordRepType,
    elx::TransformBase<TElastix>::FixedImageDimension > >,
    //elx::TransformBase<TElastix>::FixedImageDimension, __VSplineOrder > >,
  public
  TransformBase<TElastix>
{
public:

  /** Standard ITK-stuff. */
  typedef BSplineTransformWithDiffusion               Self;
  typedef DeformationFieldRegulizer<
    AdvancedCombinationTransform<
    //BSplineCombinationTransform<
    typename elx::TransformBase<TElastix>::CoordRepType,
    elx::TransformBase<TElastix>::FixedImageDimension > >
    //elx::TransformBase<TElastix>::FixedImageDimension, __VSplineOrder > >
                                                      Superclass1;
  typedef elx::TransformBase<TElastix>                Superclass2;

  /** The ITK-class that provides most of the functionality, and
   * that is set as the "CurrentTransform" in the CombinationTransform.
   */
  typedef AdvancedBSplineDeformableTransform<
    typename elx::TransformBase<TElastix>::CoordRepType,
    elx::TransformBase<TElastix>::FixedImageDimension,
    __VSplineOrder >                          BSplineTransformType;

  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( BSplineTransformWithDiffusion, DeformationFieldRegulizer );

  /** Name of this class.
   * Use this name in the parameter file to select this specific transform. \n
   * example: <tt>(Transform "BSplineTransformWithDiffusion")</tt>\n
   */
  elxClassNameMacro( "BSplineTransformWithDiffusion" );

  /** Dimension of the domain space. */
  itkStaticConstMacro( SpaceDimension, unsigned int, Superclass2::FixedImageDimension );

  /** The BSpline order. */
  itkStaticConstMacro( SplineOrder, unsigned int, __VSplineOrder );

  /** Typedefs inherited from the superclass. */
  typedef typename Superclass1::ScalarType                ScalarType;
  typedef typename Superclass1::ParametersType            ParametersType;
  typedef typename Superclass1::JacobianType              JacobianType;
  typedef typename Superclass1::InputVectorType           InputVectorType;
  typedef typename Superclass1::OutputVectorType          OutputVectorType;
  typedef typename Superclass1::InputCovariantVectorType  InputCovariantVectorType;
  typedef typename Superclass1::OutputCovariantVectorType OutputCovariantVectorType;
  typedef typename Superclass1::InputVnlVectorType        InputVnlVectorType;
  typedef typename Superclass1::OutputVnlVectorType       OutputVnlVectorType;
  typedef typename Superclass1::InputPointType            InputPointType;
  typedef typename Superclass1::OutputPointType           OutputPointType;

  /** Typedef's specific for the BSplineTransform. */
  typedef typename BSplineTransformType::PixelType                  PixelType;
  typedef typename BSplineTransformType::ImageType                  ImageType;
  typedef typename BSplineTransformType::ImagePointer               ImagePointer;
  typedef typename BSplineTransformType::RegionType                 RegionType;
  typedef typename BSplineTransformType::IndexType                  IndexType;
  typedef typename BSplineTransformType::SizeType                   SizeType;
  typedef typename BSplineTransformType::SpacingType                SpacingType;
  typedef typename BSplineTransformType::OriginType                 OriginType;
  typedef typename BSplineTransformType::BulkTransformType          BulkTransformType;
  typedef typename BSplineTransformType::BulkTransformPointer       BulkTransformPointer;
  typedef typename BSplineTransformType::WeightsFunctionType        WeightsFunctionType;
  typedef typename BSplineTransformType::WeightsType                WeightsType;
  typedef typename BSplineTransformType::ContinuousIndexType        ContinuousIndexType;
  typedef typename BSplineTransformType::ParameterIndexArrayType    ParameterIndexArrayType;

  /** Typedef's from TransformBase. */
  typedef typename Superclass2::ElastixType               ElastixType;
  typedef typename Superclass2::ElastixPointer            ElastixPointer;
  typedef typename Superclass2::ConfigurationType         ConfigurationType;
  typedef typename Superclass2::ConfigurationPointer      ConfigurationPointer;
  typedef typename Superclass2::RegistrationType          RegistrationType;
  typedef typename Superclass2::RegistrationPointer       RegistrationPointer;
  typedef typename Superclass2::CoordRepType              CoordRepType;
  typedef typename Superclass2::FixedImageType            FixedImageType;
  typedef typename Superclass2::MovingImageType           MovingImageType;
  typedef typename Superclass2::ITKBaseType               ITKBaseType;
  typedef typename Superclass2::CombinationTransformType  CombinationTransformType;

  /** Other typedef's inherited from Superclass1. */
  typedef typename Superclass1::IntermediaryDFTransformType   IntermediaryDFTransformType;
  typedef typename Superclass1::VectorImageType               VectorImageType;

  /** References to the fixed and moving image types. */
  typedef typename ElastixType::FixedImageType            FixedImageELXType;
  typedef typename ElastixType::MovingImageType           MovingImageELXType;

  /** Other typedef's.*/
  typedef Image< short,
    itkGetStaticConstMacro( SpaceDimension ) >      DummyImageType;
  typedef ImageRegionConstIterator<
    DummyImageType >                                DummyIteratorType;
  typedef typename BSplineTransformType::Pointer    BSplineTransformPointer;
  typedef typename Superclass1::Superclass          GenericDeformationFieldRegulizer;

  /** Typedef's for the diffusion of the deformation field. */
  typedef ImageFileReader< VectorImageType >        VectorReaderType;
  typedef typename VectorImageType::PixelType       VectorType;
  typedef ImageRegionIterator<
    VectorImageType >                               VectorImageIteratorType;
  typedef FixedImageELXType                         GrayValueImageType;
  typedef typename GrayValueImageType::PixelType    GrayValuePixelType;
  typedef ImageRegionIterator<
    GrayValueImageType >                            GrayValueImageIteratorType;
  typedef MaximumImageFilter<
    GrayValueImageType, GrayValueImageType,
    GrayValueImageType >                            MaximumImageFilterType;
  typedef VectorMeanDiffusionImageFilter<
    VectorImageType, GrayValueImageType >           DiffusionFilterType;
  typedef typename VectorImageType::SizeType        RadiusType;
  typedef ResampleImageFilter<
    MovingImageELXType, GrayValueImageType,
    CoordRepType >                                  ResamplerType1;
  typedef ResampleImageFilter<
    GrayValueImageType, GrayValueImageType,
    CoordRepType >                                  ResamplerType2;
  typedef BSplineInterpolateImageFunction<
    GrayValueImageType >                            InterpolatorType;
  typedef ImageFileReader< GrayValueImageType >     GrayValueImageReaderType;
  typedef ImageFileWriter< GrayValueImageType >     GrayValueImageWriterType;
  typedef ImageFileWriter< VectorImageType >        DeformationFieldWriterType;

  /** Execute stuff before the actual registration:
   * \li Create an initial B-spline grid.
   * \li Create initial registration parameters.
   * \li Setup stuff for the diffusion of the deformation field.
   */
  virtual void BeforeRegistration( void );

  /** Execute stuff before each new pyramid resolution:
   * \li upsample the B-spline grid.
   */
  virtual void BeforeEachResolution( void );

  /** Execute stuff after each iteration:
   * \li Do a diffusion of the deformation field.
   */
  virtual void AfterEachIteration( void );

  /** Execute stuff after registration:
   * \li Destroy things that are not needed anymore in order to free memory.
   */
  virtual void AfterRegistration( void );

  /** Set the initial B-spline grid. */
  virtual void SetInitialGrid( bool upsampleGridOption );

  /** Upsample the B-spline grid. */
  virtual void IncreaseScale( void );

  /** Function to read transform-parameters from a file. */
  virtual void ReadFromFile( void );

  /** Function to write transform-parameters to a file. */
  virtual void WriteToFile( const ParametersType & param ) const;

  /** Diffuse the deformation field. */
  void DiffuseDeformationField( void );

  /** Method to transform a point.
   * This method just calls the implementation from the
   * GenericDeformationFieldRegulizer. This is necessary, since:
   * The DeformationFieldRegulizerFor is used which expects
   * that its template argument is a BSplineDeformableTransform. This is
   * not the case, because we gave it a BSplineCombinationTransform.
   * This last class has a slightly different behavior of the
   * TransformPoint() method (it does not call the TransformPoint() with
   * with 5 arguments, as the BSplineDeformableTransform does).
   */
  virtual OutputPointType TransformPoint( const InputPointType  & point ) const;

  /**  Method to transform a point with extra arguments. Just calls
   * the Superclass1's implementation. Has to be present here since it is an
   * overloaded function.
   *
  virtual void TransformPoint(
    const InputPointType & inputPoint,
    OutputPointType & outputPoint,
    WeightsType & weights,
    ParameterIndexArrayType & indices,
    bool & inside ) const;*/

protected:

  /** The constructor. */
  BSplineTransformWithDiffusion();
  /** The destructor. */
  virtual ~BSplineTransformWithDiffusion() {};

  /** Member variables. */
  SpacingType   m_GridSpacingFactor;

private:

  /** The private constructor. */
  BSplineTransformWithDiffusion( const Self& ); // purposely not implemented
  /** The private copy constructor. */
  void operator=( const Self& );                // purposely not implemented

  /** Member variables for diffusion. */
  typename DiffusionFilterType::Pointer   m_Diffusion;
  typename VectorImageType::Pointer       m_DeformationField;
  typename VectorImageType::Pointer       m_DiffusedField;
  typename GrayValueImageType::Pointer    m_GrayValueImage1;
  typename GrayValueImageType::Pointer    m_GrayValueImage2;
  typename GrayValueImageType::Pointer    m_MovingSegmentationImage;
  typename GrayValueImageType::Pointer    m_FixedSegmentationImage;
  typename GrayValueImageReaderType::Pointer  m_MovingSegmentationReader;
  typename GrayValueImageReaderType::Pointer  m_FixedSegmentationReader;
  std::string                             m_MovingSegmentationFileName;
  std::string                             m_FixedSegmentationFileName;
  typename ResamplerType1::Pointer        m_Resampler1;
  typename ResamplerType2::Pointer        m_Resampler2;
  typename InterpolatorType::Pointer      m_Interpolator;
  RegionType                              m_DeformationRegion;
  OriginType                              m_DeformationOrigin;
  SpacingType                             m_DeformationSpacing;

  /** Member variables for writing diffusion files. */
  bool m_WriteDiffusionFiles;
  bool m_AlsoFixed;
  bool m_ThresholdBool;
  GrayValuePixelType m_ThresholdHU;
  bool m_UseMovingSegmentation;
  bool m_UseFixedSegmentation;

  /** The B-spline parameters, which is going to be filled with zeros. */
  ParametersType m_BSplineParameters;

  /** The internal BSplineTransform, set as a current transform in
   * the combination transform.
   */
  BSplineTransformPointer m_BSplineTransform;

}; // end class BSplineTransformWithDiffusion


} // end namespace elastix

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxBSplineTransformWithDiffusion.hxx"
#endif

#endif // end #ifndef __elxBSplineTransformWithDiffusion_H__

