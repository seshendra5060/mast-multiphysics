/*
 * MAST: Multidisciplinary-design Adaptation and Sensitivity Toolkit
 * Copyright (C) 2013-2020  Manav Bhatia and MAST authors
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

#ifndef __test__mast_mesh__
#define __test__mast_mesh__

// MAST includes
#include "base/mast_data_types.h"
#include "catch.hpp"

// libMesh includes
#include "libmesh/libmesh.h"
#include "libmesh/replicated_mesh.h"
#include "libmesh/distributed_mesh.h"
#include "libmesh/face_quad4.h"
#include "libmesh/edge_edge2.h"

// Test includes
#include "test_helpers.h"

extern libMesh::LibMeshInit* p_global_init;

namespace TEST {

    class TestMeshSingleElement {
    public:
        int n_elems;
        int n_nodes;
        int n_dim;
        libMesh::Elem* reference_elem;
        libMesh::ReplicatedMesh mesh;
        // Convert this to pointer to enable both Replicated/Distributed Mesh
        // libMesh::UnstructuredMesh* mesh;
        // ---> currently can't run this with DistributedMesh. On second processor this.reference_elem doesn't exist!

        TestMeshSingleElement(libMesh::ElemType e_type, RealMatrixX& coordinates):
                mesh(p_global_init->comm()) {
            n_elems = 1;
            n_nodes = coordinates.cols();

            mesh.reserve_elem(n_elems);
            mesh.reserve_nodes(n_nodes);
            mesh.set_spatial_dimension(3);

            for (auto i = 0; i < n_nodes; i++) {
                mesh.add_point(libMesh::Point(coordinates(0,i),coordinates(1,i), coordinates(2,i)), i, 0);
            }

            switch (e_type) {
                case libMesh::EDGE2:
                    n_dim = 1;
                    reference_elem = new libMesh::Edge2;
                    break;
                case libMesh::QUAD4:
                    n_dim = 2;
                    reference_elem = new libMesh::Quad4;
                    break;
                default:
                    libmesh_error_msg("Invalid element type; " << __PRETTY_FUNCTION__
                                                               << " in " << __FILE__ << " at line number " << __LINE__);
            }

            mesh.set_mesh_dimension(n_dim);
            reference_elem->set_id(0);
            reference_elem->subdomain_id() = 0;
            reference_elem = mesh.add_elem(reference_elem);

            for (int i=0; i<n_nodes; i++) {
                reference_elem->set_node(i) = mesh.node_ptr(i);
            }

            mesh.prepare_for_use();
        };
    };
} // TEST namespace

#endif // __test__mast_mesh__