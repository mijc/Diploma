/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __itkDeformationVectorFieldTransform_TXX__
#define __itkDeformationVectorFieldTransform_TXX__

#include "itkDeformationVectorFieldTransform.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkScalarToArrayCastImageFilter.h"

namespace itk
{


/**
 * *********************** Constructor **************************
 */

template< class TScalarType, unsigned int NDimensions >
DeformationVectorFieldTransform< TScalarType, NDimensions >
::DeformationVectorFieldTransform()
{
  /** Initialize m_Images. */
  for ( unsigned int i = 0; i < SpaceDimension; i++ )
  {
    this->m_Images[ i ] = 0;
  }

} // end Constructor


/**
* *********************** Destructor ***************************
*/

template< class TScalarType, unsigned int NDimensions >
DeformationVectorFieldTransform< TScalarType, NDimensions >
::~DeformationVectorFieldTransform()
{
  /** Initialize m_Images. */
  for ( unsigned int i = 0; i < SpaceDimension; i++ )
  {
    this->m_Images[ i ] = 0;
  }
} // end Destructor


/**
 * ******************* SetCoefficientVectorImage **********************
 *
 * Convert VectorImage (= deformation field) to series of images.
 * Set the B-Spline coefficients using a deformation field
 * image as input.
 */

template< class TScalarType, unsigned int NDimensions >
void
DeformationVectorFieldTransform< TScalarType, NDimensions >
::SetCoefficientVectorImage( const CoefficientVectorImageType * vecImage )
{
  /** Typedef's for iterators. */
  typedef ImageRegionConstIterator< CoefficientVectorImageType >  VectorIteratorType;
  typedef ImageRegionIterator< CoefficientImageType >             IteratorType;

  /** Create array of images representing the B-spline
   * coefficients in each dimension.
   */
  for ( unsigned int i = 0; i < SpaceDimension; i++ )
  {
    this->m_Images[ i ] = CoefficientImageType::New();
    this->m_Images[ i ]->SetRegions(  vecImage->GetLargestPossibleRegion() );
    this->m_Images[ i ]->SetOrigin(   vecImage->GetOrigin() );
    this->m_Images[ i ]->SetSpacing(  vecImage->GetSpacing() );
    this->m_Images[ i ]->Allocate();
  }

  /** Setup the iterators. */
  VectorIteratorType vecit( vecImage, vecImage->GetLargestPossibleRegion() );
  vecit.GoToBegin();
  IteratorType it[ SpaceDimension ];
  for ( unsigned int i = 0; i < SpaceDimension; i++ )
  {
    it[ i ] = IteratorType( this->m_Images[ i ],
      this->m_Images[ i ]->GetLargestPossibleRegion() );
    it[ i ].GoToBegin();
  }

  /** Copy one element of a vector to an image. */
  CoefficientVectorPixelType vect;
  while ( !vecit.IsAtEnd() )
  {
    vect = vecit.Get();
    for ( unsigned int i = 0; i < SpaceDimension; i++ )
    {
      it[ i ].Set( static_cast<CoefficientPixelType>( vect[ i ] ) );
      ++it[ i ];
    }
    ++vecit;
  }

  /** Put it in the Superclass. */
  this->SetCoefficientImage( this->m_Images );

} // end SetCoefficientImage()


/**
 * ******************* GetCoefficientVectorImage **********************
 *
 * Convert series of coefficient images to VectorImage (= deformation field).
 *
 */

template< class TScalarType, unsigned int NDimensions >
void
DeformationVectorFieldTransform< TScalarType, NDimensions >
::GetCoefficientVectorImage( CoefficientVectorImagePointer & vecImage ) const
{
  /** Typedef for the combiner. */
  typedef ScalarToArrayCastImageFilter<
    CoefficientImageType, CoefficientVectorImageType >      ScalarImageCombineType;

  /** Get a handle to the series of coefficient images.
   * we have to apply a painful hack, because the GetCoefficientImage method
   * is not const.
   */
  CoefficientImagePointer * coefImage;
  Self * thisNonConst = const_cast<Self *>(this);
  coefImage = thisNonConst->GetCoefficientImage();

  /** Combine the coefficient images to a vector image. */
  typename ScalarImageCombineType::Pointer combiner = ScalarImageCombineType::New();
  for ( unsigned int i = 0; i < SpaceDimension; i++ )
  {
    combiner->SetInput( i, coefImage[ i ] );
  }
  vecImage = combiner->GetOutput();
  vecImage->Update();

} // end GetCoefficientImage()


} // end namespace itk

#endif // end #ifndef __itkDeformationVectorFieldTransform_TXX__

