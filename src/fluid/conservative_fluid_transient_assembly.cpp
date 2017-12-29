/*
 * MAST: Multidisciplinary-design Adaptation and Sensitivity Toolkit
 * Copyright (C) 2013-2017  Manav Bhatia
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

// MAST includes
#include "fluid/conservative_fluid_transient_assembly.h"
#include "fluid/conservative_fluid_discipline.h"
#include "fluid/conservative_fluid_element_base.h"
#include "property_cards/element_property_card_base.h"
#include "base/physics_discipline_base.h"


MAST::ConservativeFluidTransientAssembly::
ConservativeFluidTransientAssembly():
MAST::TransientAssembly() {
    
}




MAST::ConservativeFluidTransientAssembly::
~ConservativeFluidTransientAssembly() {
    
}



void
MAST::ConservativeFluidTransientAssembly::
_elem_calculations(MAST::ElementBase& elem,
                   bool if_jac,
                   RealVectorX& f_m,
                   RealVectorX& f_x,
                   RealMatrixX& f_m_jac_xdot,
                   RealMatrixX& f_m_jac,
                   RealMatrixX& f_x_jac) {
    
    MAST::ConservativeFluidElementBase& e =
    dynamic_cast<MAST::ConservativeFluidElementBase&>(elem);
    
    f_m.setZero();
    f_x.setZero();
    f_m_jac_xdot.setZero();
    f_m_jac.setZero();
    f_x_jac.setZero();
    
    // assembly of the flux terms
    e.internal_residual(if_jac, f_x, f_x_jac);
    e.side_external_residual(if_jac, f_x, f_x_jac, _discipline->side_loads());
    
    //assembly of the capacitance term
    e.velocity_residual(if_jac, f_m, f_m_jac_xdot, f_m_jac);
}



void
MAST::ConservativeFluidTransientAssembly::
_linearized_jacobian_solution_product(MAST::ElementBase& elem,
                                      RealVectorX& f) {
    
    MAST::ConservativeFluidElementBase& e =
    dynamic_cast<MAST::ConservativeFluidElementBase&>(elem);
    
    const unsigned int
    n = (unsigned int) f.size();
    
    RealMatrixX
    dummy = RealMatrixX::Zero(n, n);
    
    f.setZero();
    
    
    e.linearized_internal_residual(false, f, dummy);
    e.linearized_side_external_residual(false, f, dummy, _discipline->side_loads());
    
    // velocity term
    e.linearized_velocity_residual(false, f, dummy, dummy);
}




void
MAST::ConservativeFluidTransientAssembly::
_elem_sensitivity_calculations(MAST::ElementBase& elem,
                               RealVectorX& vec) {
    
    libmesh_error(); // to be implemented
    
}



void
MAST::ConservativeFluidTransientAssembly::
_elem_second_derivative_dot_solution_assembly(MAST::ElementBase& elem,
                                              RealMatrixX& m) {
    
    libmesh_error(); // to be implemented
}


std::unique_ptr<MAST::ElementBase>
MAST::ConservativeFluidTransientAssembly::_build_elem(const libMesh::Elem& elem) {
    
    
    const MAST::FlightCondition& p =
    dynamic_cast<MAST::ConservativeFluidDiscipline*>(_discipline)->flight_condition();
    
    MAST::ElementBase* rval =
    new MAST::ConservativeFluidElementBase(*_system, *this, elem, p);
    
    return std::unique_ptr<MAST::ElementBase>(rval);
}

