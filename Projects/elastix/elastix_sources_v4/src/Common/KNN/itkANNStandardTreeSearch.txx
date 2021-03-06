/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __itkANNStandardTreeSearch_txx
#define __itkANNStandardTreeSearch_txx

#include "itkANNStandardTreeSearch.h"

namespace itk
{

  /**
   * ************************ Constructor *************************
   */

  template < class TBinaryTree >
    ANNStandardTreeSearch<TBinaryTree>
    ::ANNStandardTreeSearch()
  {
    this->m_ErrorBound = 0.0;
  } // end Constructor


  /**
   * ************************ Destructor *************************
   */

  template < class TBinaryTree >
    ANNStandardTreeSearch<TBinaryTree>
    ::~ANNStandardTreeSearch()
  {
  } // end Destructor


  /**
   * ************************ Search *************************
   */

  template < class TBinaryTree >
    void ANNStandardTreeSearch<TBinaryTree>
    ::Search( const MeasurementVectorType & qp, IndexArrayType & ind,
      DistanceArrayType & dists )
  {
    /** Get k , dim and eps. */
    int k       = static_cast<int>( this->m_KNearestNeighbors );
    int dim     = static_cast<int>( this->m_DataDimension );
    double eps  = this->m_ErrorBound;

    /** Allocate memory for ANN indices and distances arrays. */
    ANNIndexArrayType ANNIndices;
    ANNIndices = new ANNIndexType[ k ];

    ANNDistanceArrayType ANNDistances;
    ANNDistances = new ANNDistanceType[ k ];

    /** Alocate memory for ANN query point and copy qp to it. */
    ANNPointType ANNQueryPoint = annAllocPt( dim );
    for ( int i = 0; i < dim; i++ )
    {
      ANNQueryPoint[ i ] = qp[ i ];
    }

    /** The actual ANN search. */
    //this->m_BinaryTree->GetANNTree()->annkSearch(
      //ANNQueryPoint, k, ANNIndices, ANNDistances, eps );
    this->m_BinaryTreeAsITKANNType->GetANNTree()->annkSearch(
      ANNQueryPoint, k, ANNIndices, ANNDistances, eps );

    /** Set the ANNIndices and ANNDistances in the corresponding itk::Array's.
     * Memory management is transfered to these itk::Array's, which have SmartPointers
     * and therefore don't have to be regarded anymore. No deallocation of
     * ANNIndices and ANNDistances is needed now.
     */
    ind.SetData( ANNIndices, k, true );
    dists.SetData( ANNDistances, k, true );

    /** Deallocate the temporary ANNQueryPoint. */
    annDeallocPt( ANNQueryPoint );

  } // end Search


} // end namespace itk


#endif // end #ifndef __itkANNStandardTreeSearch_txx

