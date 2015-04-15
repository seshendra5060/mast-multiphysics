
// MAST includes
#include "elasticity/solid_element_3d.h"
#include "numerics/fem_operator_matrix.h"
#include "mesh/local_elem_base.h"
#include "property_cards/element_property_card_base.h"
#include "base/boundary_condition_base.h"
#include "base/system_initialization.h"


bool
MAST::StructuralElement3D::internal_residual(bool request_jacobian,
                                             RealVectorX& f,
                                             RealMatrixX& jac,
                                             bool if_ignore_ho_jac) {
    
    const std::vector<Real>& JxW            = _fe->get_JxW();
    const std::vector<libMesh::Point>& xyz  = _fe->get_xyz();
    const unsigned int
    n_phi              = (unsigned int)_fe->n_shape_functions(),
    n1                 =6,
    n2                 =3*n_phi,
    n3                 =30;
    
    RealMatrixX
    material_mat,
    mat_x        = RealMatrixX::Zero(6,3),
    mat_y        = RealMatrixX::Zero(6,3),
    mat_z        = RealMatrixX::Zero(6,3),
    mat1_n1n2    = RealMatrixX::Zero(n1, n2),
    mat2_n2n2    = RealMatrixX::Zero(n2, n2),
    mat3_3n2     = RealMatrixX::Zero(3, n2),
    mat4_33      = RealMatrixX::Zero(3, 3),
    mat5_n1n3    = RealMatrixX::Zero(n1, n3),
    mat6_n2n3    = RealMatrixX::Zero(n2, n3),
    mat7_3n3     = RealMatrixX::Zero(3, n3),
    Gmat         = RealMatrixX::Zero(6, n3),
    K_alphaalpha = RealMatrixX::Zero(n3, n3),
    K_ualpha     = RealMatrixX::Zero(n2, n3),
    K_corr       = RealMatrixX::Zero(n2, n2);
    RealVectorX
    strain    = RealVectorX::Zero(6),
    stress    = RealVectorX::Zero(6),
    vec1_n1   = RealVectorX::Zero(n1),
    vec2_n2   = RealVectorX::Zero(n2),
    vec3_3    = RealVectorX::Zero(3),
    alpha     = RealVectorX::Zero(n3),
    local_disp= RealVectorX::Zero(n2);
    
    // copy the values from the global to the local element
    local_disp.topRows(n2) = _local_sol.topRows(n2);
    
    std::auto_ptr<MAST::FieldFunction<RealMatrixX> > mat_stiff =
    _property.stiffness_A_matrix(*this);
    
    libMesh::Point p;
    MAST::FEMOperatorMatrix
    Bmat_lin,
    Bmat_nl_x,
    Bmat_nl_y,
    Bmat_nl_z,
    Bmat_nl_u,
    Bmat_nl_v,
    Bmat_nl_w,
    Bmat_inc;
    // six stress components, related to three displacements
    Bmat_lin.reinit(n1, 3, _elem.n_nodes());
    Bmat_nl_x.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_y.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_z.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_u.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_v.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_w.reinit(3, 3, _elem.n_nodes());
    Bmat_inc.reinit(n1, n3, 1);            // six stress-strain components

    
    ///////////////////////////////////////////////////////////////////
    // first for loop to evaluate alpha
    for (unsigned int qp=0; qp<JxW.size(); qp++) {
        
        _local_elem->global_coordinates_location(xyz[qp], p);
        
        // get the material matrix
        (*mat_stiff)(p, _time, material_mat);
        
        this->initialize_green_lagrange_strain_operator(qp,
                                                        local_disp,
                                                        strain,
                                                        mat_x, mat_y, mat_z,
                                                        Bmat_lin,
                                                        Bmat_nl_x,
                                                        Bmat_nl_y,
                                                        Bmat_nl_z,
                                                        Bmat_nl_u,
                                                        Bmat_nl_v,
                                                        Bmat_nl_w);
        this->initialize_incompatible_strain_operator(qp, Bmat_inc, Gmat);
        
        // calculate the incompatible mode matrices
        // incompatible mode diagonal stiffness matrix
        mat5_n1n3    =  material_mat * Gmat;
        K_alphaalpha += JxW[qp] * ( Gmat.transpose() * mat5_n1n3);
        
        // off-diagonal coupling matrix
        // linear strain term
        Bmat_lin.right_multiply_transpose(mat6_n2n3, mat5_n1n3);
        K_ualpha  += JxW[qp] * mat6_n2n3;
        
        // nonlinear component
        // along x
        mat7_3n3  = mat_x.transpose() * mat5_n1n3;
        Bmat_nl_x.right_multiply_transpose(mat6_n2n3, mat7_3n3);
        K_ualpha  += JxW[qp] * mat6_n2n3;
        
        // along y
        mat7_3n3  = mat_y.transpose() * mat5_n1n3;
        Bmat_nl_y.right_multiply_transpose(mat6_n2n3, mat7_3n3);
        K_ualpha  += JxW[qp] * mat6_n2n3;
        
        // along z
        mat7_3n3  = mat_z.transpose() * mat5_n1n3;
        Bmat_nl_z.right_multiply_transpose(mat6_n2n3, mat7_3n3);
        K_ualpha  += JxW[qp] * mat6_n2n3;
    }
    
    
//    // incompatible mode corrections
//    K_alphaalpha = K_alphaalpha.inverse();
//    K_corr = K_ualpha * K_alphaalpha * K_ualpha.transpose();
    
//    if (request_jacobian)
//        jac.topLeftCorner(n2, n2) -= K_corr;

//    // alpha values for use in the stress evaluation
//    alpha = -K_alphaalpha * (K_ualpha.transpose() * local_disp);

    
    
    ///////////////////////////////////////////////////////////////////////
    // second for loop to calculate the residual and stiffness contributions
    for (unsigned int qp=0; qp<JxW.size(); qp++) {
        
        _local_elem->global_coordinates_location(xyz[qp], p);
        
        // get the material matrix
        (*mat_stiff)(p, _time, material_mat);
        
        this->initialize_green_lagrange_strain_operator(qp,
                                                        local_disp,
                                                        strain,
                                                        mat_x, mat_y, mat_z,
                                                        Bmat_lin,
                                                        Bmat_nl_x,
                                                        Bmat_nl_y,
                                                        Bmat_nl_z,
                                                        Bmat_nl_u,
                                                        Bmat_nl_v,
                                                        Bmat_nl_w);
        this->initialize_incompatible_strain_operator(qp, Bmat_inc, Gmat);
        
        // calculate the stress
        stress = material_mat * (strain);// + Gmat * alpha);
        
        // calculate contribution to the residual from the
        // enhanced modes
//        f.topRows(n2) -= JxW[qp] * K_ualpha * (K_alphaalpha *
//                                               (Gmat.transpose() * stress));
        
        // calculate contribution to the residual
        // linear strain operator
        Bmat_lin.vector_mult_transpose(vec2_n2, stress);
        f.topRows(n2) += JxW[qp] * vec2_n2;
        
        // nonlinear strain operator
        // x
        vec3_3 = mat_x.transpose() * stress;
        Bmat_nl_x.vector_mult_transpose(vec2_n2, vec3_3);
        f.topRows(n2) += JxW[qp] * vec2_n2;

        // y
        vec3_3 = mat_y.transpose() * stress;
        Bmat_nl_y.vector_mult_transpose(vec2_n2, vec3_3);
        f.topRows(n2) += JxW[qp] * vec2_n2;

        // z
        vec3_3 = mat_z.transpose() * stress;
        Bmat_nl_z.vector_mult_transpose(vec2_n2, vec3_3);
        f.topRows(n2) += JxW[qp] * vec2_n2;

        
        if (request_jacobian) {
            
            // the strain includes the following expansion
            // delta_epsilon = B_lin + mat_x B_x + mat_y B_y + mat_z B_z
            // Hence, the tangent stiffness matrix will include
            // components from epsilon^T C epsilon
            
            ////////////////////////////////////////////////////////
            // B_lin^T C B_lin
            Bmat_lin.left_multiply(mat1_n1n2, material_mat);
            Bmat_lin.right_multiply_transpose(mat2_n2n2, mat1_n1n2);
            jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;
            
            // B_x^T mat_x^T C B_lin
            mat3_3n2 = mat_x.transpose() * mat1_n1n2;
            Bmat_nl_x.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;
            
            // B_y^T mat_y^T C B_lin
            mat3_3n2 = mat_y.transpose() * mat1_n1n2;
            Bmat_nl_y.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;

            // B_z^T mat_z^T C B_lin
            mat3_3n2 = mat_z.transpose() * mat1_n1n2;
            Bmat_nl_z.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;

            
            ///////////////////////////////////////////////////////
            for (unsigned int i_dim=0; i_dim<3; i_dim++) {
                switch (i_dim) {
                    case 0:
                        Bmat_nl_x.left_multiply(mat1_n1n2, mat_x);
                        break;
                        
                    case 1:
                        Bmat_nl_y.left_multiply(mat1_n1n2, mat_y);
                        break;
                        
                    case 2:
                        Bmat_nl_z.left_multiply(mat1_n1n2, mat_z);
                        break;
                }
                
                // B_lin^T C mat_x_i B_x_i
                mat1_n1n2 =  material_mat * mat1_n1n2;
                Bmat_lin.right_multiply_transpose(mat2_n2n2, mat1_n1n2);
                jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;
                
                // B_x^T mat_x^T C mat_x B_x
                mat3_3n2 = mat_x.transpose() * mat1_n1n2;
                Bmat_nl_x.right_multiply_transpose(mat2_n2n2, mat3_3n2);
                jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;
                
                // B_y^T mat_y^T C mat_x B_x
                mat3_3n2 = mat_y.transpose() * mat1_n1n2;
                Bmat_nl_y.right_multiply_transpose(mat2_n2n2, mat3_3n2);
                jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;
                
                // B_z^T mat_z^T C mat_x B_x
                mat3_3n2 = mat_z.transpose() * mat1_n1n2;
                Bmat_nl_z.right_multiply_transpose(mat2_n2n2, mat3_3n2);
                jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;
            }
            
            // use the stress to calculate the final contribution
            // to the Jacobian stiffness matrix
            mat4_33(0,0) = stress(0);
            mat4_33(1,1) = stress(1);
            mat4_33(2,2) = stress(2);
            mat4_33(0,1) = mat4_33(1,0) = stress(3);
            mat4_33(1,2) = mat4_33(2,1) = stress(4);
            mat4_33(0,2) = mat4_33(2,0) = stress(5);

            // u-disp
            Bmat_nl_u.left_multiply(mat3_3n2, mat4_33);
            Bmat_nl_u.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;
            
            // v-disp
            Bmat_nl_v.left_multiply(mat3_3n2, mat4_33);
            Bmat_nl_v.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;

            // w-disp
            Bmat_nl_w.left_multiply(mat3_3n2, mat4_33);
            Bmat_nl_w.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) += JxW[qp] * mat2_n2n2;
        }
    }
    
//    // place small values at diagonal of the rotational terms
//    if (request_jacobian) {
//        for (unsigned int i=n2/2; i<n2; i++)
//            jac(i,i) += 1.0e-12;
//    }
    
    return request_jacobian;
}



bool
MAST::StructuralElement3D::internal_residual_sensitivity(bool request_jacobian,
                                                         RealVectorX& f,
                                                         RealMatrixX& jac,
                                                         bool if_ignore_ho_jac) {
    
    return request_jacobian;
}




bool
MAST::StructuralElement3D::prestress_residual (bool request_jacobian,
                                               RealVectorX& f,
                                               RealMatrixX& jac) {
    
    return request_jacobian;
}



bool
MAST::StructuralElement3D::prestress_residual_sensitivity (bool request_jacobian,
                                                           RealVectorX& f,
                                                           RealMatrixX& jac) {
    
    return request_jacobian;
}




bool
MAST::StructuralElement3D::thermal_residual(bool request_jacobian,
                                            RealVectorX& f,
                                            RealMatrixX& jac,
                                            MAST::BoundaryConditionBase& bc) {
    
    const std::vector<Real>& JxW            = _fe->get_JxW();
    const std::vector<libMesh::Point>& xyz  = _fe->get_xyz();
    
    const unsigned int
    n_phi = (unsigned int)_fe->get_phi().size(),
    n1    = 6,
    n2    = 3*n_phi;
    
    RealMatrixX
    material_exp_A_mat,
    mat_x        = RealMatrixX::Zero(6,3),
    mat_y        = RealMatrixX::Zero(6,3),
    mat_z        = RealMatrixX::Zero(6,3),
    mat2_n2n2    = RealMatrixX::Zero(n2,n2),
    mat3_3n2     = RealMatrixX::Zero(3,n2),
    mat4_33      = RealMatrixX::Zero(3,3);
    RealVectorX
    vec1_n1   = RealVectorX::Zero(n1),
    vec2_3    = RealVectorX::Zero(3),
    vec3_n2   = RealVectorX::Zero(n2),
    delta_t   = RealVectorX::Zero(1),
    local_disp= RealVectorX::Zero(n2),
    strain    = RealVectorX::Zero(6);

    // copy the values from the global to the local element
    local_disp.topRows(n2) = _local_sol.topRows(n2);
    
    libMesh::Point p;

    MAST::FEMOperatorMatrix
    Bmat_lin,
    Bmat_nl_x,
    Bmat_nl_y,
    Bmat_nl_z,
    Bmat_nl_u,
    Bmat_nl_v,
    Bmat_nl_w;
    // six stress components, related to three displacements
    Bmat_lin.reinit(n1, 3, _elem.n_nodes());
    Bmat_nl_x.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_y.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_z.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_u.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_v.reinit(3, 3, _elem.n_nodes());
    Bmat_nl_w.reinit(3, 3, _elem.n_nodes());

    std::auto_ptr<MAST::FieldFunction<RealMatrixX> >
    mat = _property.thermal_expansion_A_matrix(*this);
    
    const MAST::FieldFunction<Real>
    &temp_func     = bc.get<MAST::FieldFunction<Real> >("temperature"),
    &ref_temp_func = bc.get<MAST::FieldFunction<Real> >("ref_temperature");
    
    Real t, t0;
    
    for (unsigned int qp=0; qp<JxW.size(); qp++) {
        
        _local_elem->global_coordinates_location(xyz[qp], p);
        
        (*mat)       (p, _time, material_exp_A_mat);
        temp_func    (p, _time, t);
        ref_temp_func(p, _time, t0);
        delta_t(0) = t-t0;
        
        vec1_n1 = material_exp_A_mat * delta_t; // [C]{alpha (T - T0)}
        
        this->initialize_green_lagrange_strain_operator(qp,
                                                        local_disp,
                                                        strain,
                                                        mat_x, mat_y, mat_z,
                                                        Bmat_lin,
                                                        Bmat_nl_x,
                                                        Bmat_nl_y,
                                                        Bmat_nl_z,
                                                        Bmat_nl_u,
                                                        Bmat_nl_v,
                                                        Bmat_nl_w);
        
        // linear strain operotor
        Bmat_lin.vector_mult_transpose(vec3_n2, vec1_n1);
        f.topRows(n2) -= JxW[qp] * vec3_n2;
        
        // nonlinear strain operotor
        // x
        vec2_3 = mat_x.transpose() * vec1_n1;
        Bmat_nl_x.vector_mult_transpose(vec3_n2, vec2_3);
        f.topRows(n2) -= JxW[qp] * vec3_n2;

        // y
        vec2_3 = mat_y.transpose() * vec1_n1;
        Bmat_nl_y.vector_mult_transpose(vec3_n2, vec2_3);
        f.topRows(n2) -= JxW[qp] * vec3_n2;

        // z
        vec2_3 = mat_z.transpose() * vec1_n1;
        Bmat_nl_z.vector_mult_transpose(vec3_n2, vec2_3);
        f.topRows(n2) -= JxW[qp] * vec3_n2;
        
        // Jacobian for the nonlinear case
        if (request_jacobian) {

            // use the stress to calculate the final contribution
            // to the Jacobian stiffness matrix
            mat4_33(0,0) = vec1_n1(0);
            mat4_33(1,1) = vec1_n1(1);
            mat4_33(2,2) = vec1_n1(2);
            mat4_33(0,1) = mat4_33(1,0) = vec1_n1(3);
            mat4_33(1,2) = mat4_33(2,1) = vec1_n1(4);
            mat4_33(0,2) = mat4_33(2,0) = vec1_n1(5);
            
            // u-disp
            Bmat_nl_u.left_multiply(mat3_3n2, mat4_33);
            Bmat_nl_u.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) -= JxW[qp] * mat2_n2n2;
            
            // v-disp
            Bmat_nl_v.left_multiply(mat3_3n2, mat4_33);
            Bmat_nl_v.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) -= JxW[qp] * mat2_n2n2;
            
            // w-disp
            Bmat_nl_w.left_multiply(mat3_3n2, mat4_33);
            Bmat_nl_w.right_multiply_transpose(mat2_n2n2, mat3_3n2);
            jac.topLeftCorner(n2, n2) -= JxW[qp] * mat2_n2n2;
        }
    }
    
    // Jacobian contribution from von Karman strain
    return request_jacobian;
}



bool
MAST::StructuralElement3D::thermal_residual_sensitivity(bool request_jacobian,
                                                        RealVectorX& f,
                                                        RealMatrixX& jac,
                                                        MAST::BoundaryConditionBase& p) {
    
    return false;
}


void
MAST::StructuralElement3D::initialize_strain_operator (const unsigned int qp,
                                                       FEMOperatorMatrix& Bmat) {
    
    const std::vector<std::vector<libMesh::RealVectorValue> >&
    dphi = _fe->get_dphi();
    
    unsigned int n_phi = (unsigned int)dphi.size();
    RealVectorX phi  = RealVectorX::Zero(n_phi);
    
    // now set the shape function values
    // dN/dx
    for ( unsigned int i_nd=0; i_nd<n_phi; i_nd++ )
        phi(i_nd) = dphi[i_nd][qp](0);
    Bmat.set_shape_function(0, 0, phi); //  epsilon_xx = du/dx
    Bmat.set_shape_function(3, 1, phi); //  gamma_xy = dv/dx + ...
    Bmat.set_shape_function(5, 2, phi); //  gamma_zx = dw/dx + ...
    
    // dN/dy
    for ( unsigned int i_nd=0; i_nd<n_phi; i_nd++ )
        phi(i_nd) = dphi[i_nd][qp](1);
    Bmat.set_shape_function(1, 1, phi); //  epsilon_yy = dv/dy
    Bmat.set_shape_function(3, 0, phi); //  gamma_xy = du/dy + ...
    Bmat.set_shape_function(4, 2, phi); //  gamma_yz = dw/dy + ...
    
    // dN/dz
    for ( unsigned int i_nd=0; i_nd<n_phi; i_nd++ )
        phi(i_nd) = dphi[i_nd][qp](2);
    Bmat.set_shape_function(2, 2, phi); //  epsilon_xx = dw/dz
    Bmat.set_shape_function(4, 1, phi); //  gamma_xy = dv/dz + ...
    Bmat.set_shape_function(5, 0, phi); //  gamma_zx = du/dz + ...
}



void
MAST::StructuralElement3D::
initialize_green_lagrange_strain_operator(const unsigned int qp,
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
                                          MAST::FEMOperatorMatrix& Bmat_nl_w) {
    

    
    const std::vector<std::vector<libMesh::RealVectorValue> >&
    dphi = _fe->get_dphi();
    
    unsigned int n_phi = (unsigned int)dphi.size();
    RealVectorX phi  = RealVectorX::Zero(n_phi);

    // make sure all matrices are the right size
    libmesh_assert_equal_to(epsilon.size(), 6);
    libmesh_assert_equal_to(mat_x.rows(), 6);
    libmesh_assert_equal_to(mat_x.cols(), 3);
    libmesh_assert_equal_to(mat_y.rows(), 6);
    libmesh_assert_equal_to(mat_y.cols(), 3);
    libmesh_assert_equal_to(mat_z.rows(), 6);
    libmesh_assert_equal_to(mat_z.cols(), 3);
    libmesh_assert_equal_to(Bmat_lin.m(), 6);
    libmesh_assert_equal_to(Bmat_lin.n(), 3*n_phi);
    libmesh_assert_equal_to(Bmat_nl_x.m(), 3);
    libmesh_assert_equal_to(Bmat_nl_x.n(), 3*n_phi);
    libmesh_assert_equal_to(Bmat_nl_y.m(), 3);
    libmesh_assert_equal_to(Bmat_nl_y.n(), 3*n_phi);
    libmesh_assert_equal_to(Bmat_nl_z.m(), 3);
    libmesh_assert_equal_to(Bmat_nl_z.n(), 3*n_phi);

    
    // now set the shape function values
    // dN/dx
    for ( unsigned int i_nd=0; i_nd<n_phi; i_nd++ )
        phi(i_nd) = dphi[i_nd][qp](0);
    // linear strain operator
    Bmat_lin.set_shape_function(0, 0, phi); //  epsilon_xx = du/dx
    Bmat_lin.set_shape_function(3, 1, phi); //  gamma_xy = dv/dx + ...
    Bmat_lin.set_shape_function(5, 2, phi); //  gamma_zx = dw/dx + ...
    
    // nonlinear strain operator in x
    Bmat_nl_x.set_shape_function(0, 0, phi); // du/dx
    Bmat_nl_x.set_shape_function(1, 1, phi); // dv/dx
    Bmat_nl_x.set_shape_function(2, 2, phi); // dw/dx

    // nonlinear strain operator in u
    Bmat_nl_u.set_shape_function(0, 0, phi); // du/dx
    Bmat_nl_v.set_shape_function(0, 1, phi); // dv/dx
    Bmat_nl_w.set_shape_function(0, 2, phi); // dw/dx

    
    // dN/dy
    for ( unsigned int i_nd=0; i_nd<n_phi; i_nd++ )
        phi(i_nd) = dphi[i_nd][qp](1);
    // linear strain operator
    Bmat_lin.set_shape_function(1, 1, phi); //  epsilon_yy = dv/dy
    Bmat_lin.set_shape_function(3, 0, phi); //  gamma_xy = du/dy + ...
    Bmat_lin.set_shape_function(4, 2, phi); //  gamma_yz = dw/dy + ...
    
    // nonlinear strain operator in y
    Bmat_nl_y.set_shape_function(0, 0, phi); // du/dy
    Bmat_nl_y.set_shape_function(1, 1, phi); // dv/dy
    Bmat_nl_y.set_shape_function(2, 2, phi); // dw/dy

    // nonlinear strain operator in v
    Bmat_nl_u.set_shape_function(1, 0, phi); // du/dy
    Bmat_nl_v.set_shape_function(1, 1, phi); // dv/dy
    Bmat_nl_w.set_shape_function(1, 2, phi); // dw/dy

    // dN/dz
    for ( unsigned int i_nd=0; i_nd<n_phi; i_nd++ )
        phi(i_nd) = dphi[i_nd][qp](2);
    Bmat_lin.set_shape_function(2, 2, phi); //  epsilon_xx = dw/dz
    Bmat_lin.set_shape_function(4, 1, phi); //  gamma_xy = dv/dz + ...
    Bmat_lin.set_shape_function(5, 0, phi); //  gamma_zx = du/dz + ...

    // nonlinear strain operator in z
    Bmat_nl_z.set_shape_function(0, 0, phi); // du/dz
    Bmat_nl_z.set_shape_function(1, 1, phi); // dv/dz
    Bmat_nl_z.set_shape_function(2, 2, phi); // dw/dz

    // nonlinear strain operator in w
    Bmat_nl_u.set_shape_function(2, 0, phi); // du/dz
    Bmat_nl_v.set_shape_function(2, 1, phi); // dv/dz
    Bmat_nl_w.set_shape_function(2, 2, phi); // dw/dz

    
    // calculate the displacement gradient to create the
    RealVectorX
    ddisp_dx = RealVectorX::Zero(3),
    ddisp_dy = RealVectorX::Zero(3),
    ddisp_dz = RealVectorX::Zero(3);
    
    Bmat_nl_x.vector_mult(ddisp_dx, local_disp);  // {du/dx, dv/dx, dw/dx}
    Bmat_nl_y.vector_mult(ddisp_dy, local_disp);  // {du/dy, dv/dy, dw/dy}
    Bmat_nl_z.vector_mult(ddisp_dz, local_disp);  // {du/dz, dv/dz, dw/dz}

    // prepare the deformation gradient matrix
    RealMatrixX
    F = RealMatrixX::Zero(3,3),
    E = RealMatrixX::Zero(3,3);
    F.col(0) = ddisp_dx;
    F.col(1) = ddisp_dy;
    F.col(2) = ddisp_dz;
    
    // this calculates the Green-Lagrange strain in the reference config
    E = 0.5*(F + F.transpose() + F.transpose() * F);
    
    // now, add this to the strain vector
    epsilon(0) = E(0,0);
    epsilon(1) = E(1,1);
    epsilon(2) = E(2,2);
    epsilon(3) = E(0,1) + E(1,0);
    epsilon(4) = E(1,2) + E(2,1);
    epsilon(5) = E(0,2) + E(2,0);
    
    // now initialize the matrices with strain components
    // that multiply the Bmat_nl terms
    mat_x.row(0) =     ddisp_dx;
    mat_x.row(3) =     ddisp_dy;
    mat_x.row(5) =     ddisp_dz;
    
    mat_y.row(1) =     ddisp_dy;
    mat_y.row(3) =     ddisp_dx;
    mat_y.row(4) =     ddisp_dz;

    
    mat_z.row(2) =     ddisp_dz;
    mat_z.row(4) =     ddisp_dy;
    mat_z.row(5) =     ddisp_dx;

}




void
MAST::StructuralElement3D::initialize_incompatible_strain_operator(const unsigned int qp,
                                                                   FEMOperatorMatrix& Bmat,
                                                                   RealMatrixX& G_mat) {
    
    RealVectorX phi_vec = RealVectorX::Zero(1);
    
    // get the location of element coordinates
    const std::vector<libMesh::Point>& q_point = _qrule->get_points();
    const Real
    xi  = q_point[qp](0),
    eta = q_point[qp](1),
    phi = q_point[qp](2);
    
    const std::vector<libMesh::RealGradient>&
    dxyzdxi  = _fe->get_dxyzdxi(),
    dxyzdeta = _fe->get_dxyzdeta(),
    dxyzdphi = _fe->get_dxyzdzeta();
    
    RealMatrixX
    jac = RealMatrixX::Zero(3,3);
    
    jac(0,0) =  dxyzdxi[qp](0);
    jac(0,1) =  dxyzdxi[qp](1);
    jac(0,2) =  dxyzdxi[qp](2);
    
    jac(1,0) =  dxyzdeta[qp](0);
    jac(1,1) =  dxyzdeta[qp](1);
    jac(1,2) =  dxyzdeta[qp](2);
    
    jac(2,0) =  dxyzdphi[qp](0);
    jac(2,1) =  dxyzdphi[qp](1);
    jac(2,2) =  dxyzdphi[qp](2);

    // now set the shape function values
    // epsilon_xx
    phi_vec(0) =         xi;   Bmat.set_shape_function(0,  0, phi_vec);
    phi_vec(0) =     xi*eta;   Bmat.set_shape_function(0, 15, phi_vec);
    phi_vec(0) =     xi*phi;   Bmat.set_shape_function(0, 16, phi_vec);
    phi_vec(0) = xi*eta*phi;   Bmat.set_shape_function(0, 24, phi_vec);

    
    // epsilon_yy
    phi_vec(0) =        eta;   Bmat.set_shape_function(1,  1, phi_vec);
    phi_vec(0) =     xi*eta;   Bmat.set_shape_function(1, 17, phi_vec);
    phi_vec(0) =    eta*phi;   Bmat.set_shape_function(1, 18, phi_vec);
    phi_vec(0) = xi*eta*phi;   Bmat.set_shape_function(1, 25, phi_vec);

    // epsilon_zz
    phi_vec(0) =        phi;   Bmat.set_shape_function(2,  2, phi_vec);
    phi_vec(0) =     xi*phi;   Bmat.set_shape_function(2, 19, phi_vec);
    phi_vec(0) =    eta*phi;   Bmat.set_shape_function(2, 20, phi_vec);
    phi_vec(0) = xi*eta*phi;   Bmat.set_shape_function(2, 26, phi_vec);

    // epsilon_xy
    phi_vec(0) =         xi;   Bmat.set_shape_function(3,  3, phi_vec);
    phi_vec(0) =        eta;   Bmat.set_shape_function(3,  4, phi_vec);
    phi_vec(0) =     xi*phi;   Bmat.set_shape_function(3,  9, phi_vec);
    phi_vec(0) =    eta*phi;   Bmat.set_shape_function(3, 10, phi_vec);
    phi_vec(0) =     xi*eta;   Bmat.set_shape_function(3, 21, phi_vec);
    phi_vec(0) = xi*eta*phi;   Bmat.set_shape_function(3, 27, phi_vec);

    // epsilon_yz
    phi_vec(0) =        eta;   Bmat.set_shape_function(4,  5, phi_vec);
    phi_vec(0) =        phi;   Bmat.set_shape_function(4,  6, phi_vec);
    phi_vec(0) =     xi*eta;   Bmat.set_shape_function(4, 11, phi_vec);
    phi_vec(0) =     xi*phi;   Bmat.set_shape_function(4, 12, phi_vec);
    phi_vec(0) =    eta*phi;   Bmat.set_shape_function(4, 22, phi_vec);
    phi_vec(0) = xi*eta*phi;   Bmat.set_shape_function(4, 28, phi_vec);

    // epsilon_xz
    phi_vec(0) =         xi;   Bmat.set_shape_function(5,  7, phi_vec);
    phi_vec(0) =        phi;   Bmat.set_shape_function(5,  8, phi_vec);
    phi_vec(0) =     xi*eta;   Bmat.set_shape_function(5, 13, phi_vec);
    phi_vec(0) =    eta*phi;   Bmat.set_shape_function(5, 14, phi_vec);
    phi_vec(0) =     xi*phi;   Bmat.set_shape_function(5, 23, phi_vec);
    phi_vec(0) = xi*eta*phi;   Bmat.set_shape_function(5, 29, phi_vec);
    
    Bmat.left_multiply(G_mat, _T0_inv_tr);
    G_mat *= jac.determinant();
}




void
MAST::StructuralElement3D::_init_incompatible_fe_mapping( const libMesh::Elem& e) {
    
    unsigned int nv = _system.system().n_vars();
    
    libmesh_assert (nv);
    libMesh::FEType fe_type = _system.system().variable_type(0); // all variables are assumed to be of same type
    
    
    for (unsigned int i=1; i != nv; ++i)
        libmesh_assert(fe_type == _system.system().variable_type(i));
    
    // Create an adequate quadrature rule
    std::auto_ptr<libMesh::FEBase> fe(libMesh::FEBase::build(e.dim(), fe_type).release());
    const std::vector<libMesh::Point> pts(1); // initializes point to (0,0,0)
    
    fe->get_dxyzdxi();
    fe->get_dxyzdeta();
    fe->get_dxyzdzeta();
    
    fe->reinit(&e, &pts); // reinit at (0,0,0)
    
    _T0_inv_tr = RealMatrixX::Zero(6,6);
    
    const std::vector<libMesh::RealGradient>&
    dxyzdxi  = fe->get_dxyzdxi(),
    dxyzdeta = fe->get_dxyzdeta(),
    dxyzdphi = fe->get_dxyzdzeta();

    RealMatrixX
    jac = RealMatrixX::Zero(3,3),
    T0  = RealMatrixX::Zero(6,6);
    
    jac(0,0) =  dxyzdxi[0](0);
    jac(0,1) =  dxyzdxi[0](1);
    jac(0,2) =  dxyzdxi[0](2);
    
    jac(1,0) =  dxyzdeta[0](0);
    jac(1,1) =  dxyzdeta[0](1);
    jac(1,2) =  dxyzdeta[0](2);
    
    jac(2,0) =  dxyzdphi[0](0);
    jac(2,1) =  dxyzdphi[0](1);
    jac(2,2) =  dxyzdphi[0](2);
    
    // we first set the values of the T0 matrix and then get its inverse
    T0(0,0) =   jac(0,0)*jac(0,0);
    T0(0,1) =   jac(1,0)*jac(1,0);
    T0(0,2) =   jac(2,0)*jac(2,0);
    T0(0,3) = 2*jac(0,0)*jac(1,0);
    T0(0,4) = 2*jac(1,0)*jac(2,0);
    T0(0,5) = 2*jac(2,0)*jac(0,0);
    
    T0(1,0) =   jac(0,1)*jac(0,1);
    T0(1,1) =   jac(1,1)*jac(1,1);
    T0(1,2) =   jac(2,1)*jac(2,1);
    T0(1,3) = 2*jac(0,1)*jac(1,1);
    T0(1,4) = 2*jac(1,1)*jac(2,1);
    T0(1,5) = 2*jac(2,1)*jac(0,1);

    T0(2,0) =   jac(0,2)*jac(0,2);
    T0(2,1) =   jac(1,2)*jac(1,2);
    T0(2,2) =   jac(2,2)*jac(2,2);
    T0(2,3) = 2*jac(0,2)*jac(1,2);
    T0(2,4) = 2*jac(1,2)*jac(2,2);
    T0(2,5) = 2*jac(2,2)*jac(0,2);

    T0(3,0) =   jac(0,0)*jac(0,1);
    T0(3,1) =   jac(1,0)*jac(1,1);
    T0(3,2) =   jac(2,0)*jac(2,1);
    T0(3,3) =   jac(0,0)*jac(1,1)+jac(1,0)*jac(0,1);
    T0(3,4) =   jac(1,0)*jac(2,1)+jac(2,0)*jac(1,1);
    T0(3,5) =   jac(2,1)*jac(0,0)+jac(2,0)*jac(0,1);

    T0(4,0) =   jac(0,1)*jac(0,2);
    T0(4,1) =   jac(1,1)*jac(1,2);
    T0(4,2) =   jac(2,1)*jac(2,2);
    T0(4,3) =   jac(0,1)*jac(1,2)+jac(1,1)*jac(0,2);
    T0(4,4) =   jac(1,1)*jac(2,2)+jac(2,1)*jac(1,2);
    T0(4,5) =   jac(2,2)*jac(0,1)+jac(2,1)*jac(0,2);

    T0(5,0) =   jac(0,0)*jac(0,2);
    T0(5,1) =   jac(1,0)*jac(1,2);
    T0(5,2) =   jac(2,0)*jac(2,2);
    T0(5,3) =   jac(0,0)*jac(1,2)+jac(1,0)*jac(0,2);
    T0(5,4) =   jac(1,0)*jac(2,2)+jac(2,0)*jac(1,2);
    T0(5,5) =   jac(2,2)*jac(0,0)+jac(2,0)*jac(0,2);
    
    _T0_inv_tr = jac.determinant() * T0.inverse().transpose();
}
