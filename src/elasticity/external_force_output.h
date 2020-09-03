/*
 * MAST: Multidisciplinary-design Adaptation and Sensitivity Toolkit
 * Copyright (C) 2013-2019  Manav Bhatia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __mast__external_force_output__
#define __mast__external_force_output__

// C++ includes
#include <map>
#include <vector>

// MAST includes
#include "base/mast_data_types.h"
#include "base/physics_discipline_base.h"
#include "base/output_assembly_elem_operations.h"


// libMesh includes
#include "libmesh/elem.h"

namespace MAST {
    
    
    // Forward declerations
    class FunctionBase;
    
    
    /*!
     * 
     */
    class ExternalForceOutput:
    public MAST::OutputAssemblyElemOperations {
        
    public:
        
        /*!
         *    default constructor
         */
        ExternalForceOutput(const std::vector<Real> w);
        
        virtual ~ExternalForceOutput();
        
        
        /*!
         *   sets the structural element y-vector if 1D element is used.
         */
        virtual void
        set_elem_data(unsigned int dim,
                      const libMesh::Elem& ref_elem,
                      MAST::GeomElem& elem) const;
        
        /*!
         *   initialize for the element.
         */
        virtual void init(const MAST::GeomElem& elem);
        
        /*!
         *   zeroes the output quantity values stored inside this object
         *   so that assembly process can begin. This will zero out data
         *   so that it is ready for a new evaluation. Before sensitivity
         *   analysis, call the other method, since some nonlinear
         *   functionals need the forward quantities for sensitivity analysis,
         *   eg., stress output.
         */
        virtual void zero_for_analysis();
        
        
        /*!
         *   zeroes the output quantity values stored inside this object
         *   so that assembly process can begin. This will only zero the
         *   data to compute new sensitivity analysis.
         */
        virtual void zero_for_sensitivity();
        
        /*!
         *    this evaluates all relevant stress components on the element to
         *    evaluate the p-averaged quantity.
         *    This is only done on the current element for which this
         *    object has been initialized.
         */
        virtual void evaluate();
        
        /*!
         *    this evaluates all relevant stress sensitivity components on
         *    the element to evaluate the p-averaged quantity sensitivity.
         *    This is only done on the current element for which this
         *    object has been initialized.
         */
        virtual void evaluate_sensitivity(const MAST::FunctionBase& f);
        
        /*!
         *    this evaluates all relevant shape sensitivity components on
         *    the element.
         *    This is only done on the current element for which this
         *    object has been initialized.
         */
        virtual void evaluate_shape_sensitivity(const MAST::FunctionBase& f) {
            
            libmesh_assert(false); // to be implemented
        }
        
        /*!
         *    This evaluates the contribution to the topology sensitivity on the
         *    boundary. Given that the integral is nonlinear due to the \f$p-\f$norm,
         *    the expression is quite involved:
         *   \f[  \frac{ \frac{1}{p} \left( \int_\Omega (\sigma_{VM}(\Omega))^p ~
         *    d\Omega \right)^{\frac{1}{p}-1}}{\left(  \int_\Omega ~ d\Omega \right)^{\frac{1}{p}}}
         *    \int_\Gamma  V_n \sigma_{VM}^p  ~d\Gamma +
         *    \frac{ \frac{-1}{p} \left( \int_\Omega (\sigma_{VM}(\Omega))^p ~
         *    d\Omega \right)^{\frac{1}{p}}}{\left(  \int_\Omega ~ d\Omega \right)^{\frac{1+p}{p}}}
         *    \int_\Gamma  V_n ~d\Gamma \f]
         */
        virtual void
        evaluate_topology_sensitivity(const MAST::FunctionBase& f,
                                      const MAST::FieldFunction<RealVectorX>& vel);
        
        /*!
         *   should not get called for this output. Use output_total() instead.
         */
        virtual Real output_for_elem() {
            //
            libmesh_error();
        }
        
        /*!
         *   @returns the output quantity value accumulated over all elements
         */
        virtual Real output_total();
        
        /*!
         *   @returns the sensitivity of p-norm von Mises stress for the
         *   \f$p-\f$norm identified using \p set_p_val(). The returned quantity
         *   is evaluated for the element for which this object is initialized.
         */
        virtual Real output_sensitivity_for_elem(const MAST::FunctionBase& p) {
            libmesh_error(); // not yet implemented
        }
        
        
        /*!
         *   @returns the output quantity sensitivity for parameter.
         *   This method calculates the partial derivative of quantity
         *    \f[ \frac{\partial q(X, p)}{\partial p} \f]  with
         *    respect to parameter \f$ p \f$. This returns the quantity
         *   accumulated over all elements.
         */
        virtual Real output_sensitivity_total(const MAST::FunctionBase& p);
        
        
        /*!
         *   This method calculates the partial derivative of the output
         *   quantity with respect to the parameter 
         *   \f[ \frac{\partial q(X, p)}{\partial X} \f]
         *   evaluated over the current element for which this object
         *   is initialized.
         */
        virtual void output_derivative_for_elem(RealVectorX& dq_dX);
        
        virtual void evaluate_for_node(const RealVectorX& Xnode,
                                       const RealVectorX& Fpnode) override;
        
        /*!
         *   This method calculates the partial derivative of the output
         *   quantity with respect to the parameter 
         *   \f[ \frac{\partial q(X, p)}{\partial X} \f]
         *   evaluated over the current node for which this object
         *   is initialized.
         */
        virtual void output_derivative_for_node(const RealVectorX& Xnode, 
            const RealVectorX& Fpnode, RealVectorX& dq_dX) override;
        
        /*!
         * This method calculates the total derivative of the output quantity
         * with respect to the parameter 
         * \f[ \frac{\delta q(X, p)}{\delta p} \f]
         * 
         * If \f$ dXnode_dparam \f$ is a nullptr, then this method instead
         * calculates the partial derivative of the output quantity with 
         * respect to the parameter
         * \f[ \frac{\partial q(X, p)}{\partial p} \f]
         * 
         * Evaluated over the current node for which this object is initialized.
         */
        virtual void evaluate_sensitivity_for_node(const MAST::FunctionBase& f,
            const RealVectorX& Xnode, const RealVectorX& dXnode_dparam,
            const RealVectorX& Fpnode, const RealVectorX& dpF_fpparam_node) override;
        
        
    protected:
    
        Real _external_force;
        Real _dexternal_force_dp;
        std::vector<Real> _w; // Weights for linear combination of external forces
    };
}

#endif // __mast__external_force_output__