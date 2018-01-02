/*
 * MAST: Multidisciplinary-design Adaptation and Sensitivity Toolkit
 * Copyright (C) 2013-2018  Manav Bhatia
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
#include "base/eigenproblem_assembly_elem_operations.h"
#include "base/elem_base.h"


MAST::EigenproblemAssemblyElemOperations::EigenproblemAssemblyElemOperations():
MAST::AssemblyElemOperations() {
    
}


MAST::EigenproblemAssemblyElemOperations::~EigenproblemAssemblyElemOperations() {
    
}


void
MAST::EigenproblemAssemblyElemOperations::
set_elem_sol(MAST::ElementBase& elem,
             const RealVectorX& sol) {
    
    elem.set_solution(sol);
}


void
MAST::EigenproblemAssemblyElemOperations::
set_elem_sol_sens(MAST::ElementBase& elem,
                  const RealVectorX& sol) {
    
    elem.set_solution(sol, true);
}
