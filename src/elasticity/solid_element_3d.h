//
//  solid_element_3d.h
//  mast
//
//  Created by Manav Bhatia on 2/17/15.
//  Copyright (c) 2015 MAST. All rights reserved.
//

#ifndef __mast__solid_element_3d__
#define __mast__solid_element_3d__

/// MAST includes
#include "elasticity/structural_element_base.h"

// Forward declerations
class FEMOperatorMatrix;

namespace MAST {
    
    // Forward declerations
    class BoundaryConditionBase;
    
    
    class StructuralElement3D:
    public MAST::StructuralElementBase {
        
    public:
        StructuralElement3D(MAST::SystemInitialization& sys,
                            const libMesh::Elem& elem,
                            const MAST::ElementPropertyCardBase& p):
        StructuralElementBase(sys, elem, p) {
            
            _init_incompatible_fe_mapping(elem);
        }
        
        /*!
         *    Calculates the internal residual vector and Jacobian due to
         *    strain energy
         */
        virtual bool internal_residual(bool request_jacobian,
                                       RealVectorX& f,
                                       RealMatrixX& jac,
                                       bool if_ignore_ho_jac);
        
        
        /*!
         *    Calculates the sensitivity of internal residual vector and
         *    Jacobian due to strain energy
         */
        virtual bool internal_residual_sensitivity(bool request_jacobian,
                                                   RealVectorX& f,
                                                   RealMatrixX& jac,
                                                   bool if_ignore_ho_jac);
        
        /*!
         *   calculates d[J]/d{x} . d{x}/dp
         */
        virtual bool
        internal_residual_jac_dot_state_sensitivity (RealMatrixX& jac) {
            libmesh_assert(false); // to be implemented for 3D elements
        }
        
        /*!
         *    Calculates the prestress residual vector and Jacobian
         */
        virtual bool prestress_residual (bool request_jacobian,
                                         RealVectorX& f,
                                         RealMatrixX& jac);
        
        
        /*!
         *    Calculates the sensitivity prestress residual vector and Jacobian
         */
        virtual bool prestress_residual_sensitivity (bool request_jacobian,
                                                     RealVectorX& f,
                                                     RealMatrixX& jac);
        
        
    protected:
        
        /*!
         *    Calculates the residual vector and Jacobian due to thermal stresses
         */
        virtual bool thermal_residual(bool request_jacobian,
                                      RealVectorX& f,
                                      RealMatrixX& jac,
                                      MAST::BoundaryConditionBase& p);
        
        /*!
         *    Calculates the sensitivity of residual vector and Jacobian due to
         *    thermal stresses
         */
        virtual bool thermal_residual_sensitivity(bool request_jacobian,
                                                  RealVectorX& f,
                                                  RealMatrixX& jac,
                                                  MAST::BoundaryConditionBase& p);
        
        /*!
         *   initialize strain operator matrix
         */
        void initialize_strain_operator (const unsigned int qp,
                                         FEMOperatorMatrix& Bmat);
        
        /*!
         *    initialize the strain operator matrices for the 
         *    Green-Lagrange strain matrices
         */
        void initialize_green_lagrange_strain_operator(const unsigned int qp,
                                                       const RealVectorX& local_disp,
                                                       RealVectorX& epsilon,
                                                       RealMatrixX& mat_x,
                                                       RealMatrixX& mat_y,
                                                       RealMatrixX& mat_z,
                                                       MAST::FEMOperatorMatrix& Bmat_lin,
                                                       MAST::FEMOperatorMatrix& Bmat_nl_x,
                                                       MAST::FEMOperatorMatrix& Bmat_nl_y,
                                                       MAST::FEMOperatorMatrix& Bmat_nl_z,
                                                       MAST::FEMOperatorMatrix& Bmat_nl_u,
                                                       MAST::FEMOperatorMatrix& Bmat_nl_v,
                                                       MAST::FEMOperatorMatrix& Bmat_nl_w);
        
        /*!
         *   initialize incompatible strain operator
         */
        void initialize_incompatible_strain_operator(const unsigned int qp,
                                                     FEMOperatorMatrix& Bmat,
                                                     RealMatrixX& G_mat);

        /*!
         *   initialize the Jacobian needed for incompatible modes
         */
        void _init_incompatible_fe_mapping( const libMesh::Elem& e);

        
        /*!
         *   Jacobian matrix at element center needed for incompatible modes
         */
        RealMatrixX _T0_inv_tr;

    };
}

#endif // __mast__solid_element_3d__