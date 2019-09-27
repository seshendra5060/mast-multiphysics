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


// MAST includes
#include "property_cards/solid_1d_Z_section_element_property_card.h"


// Z-Beam (Z in Nastran)
void MAST::Solid1DZSectionProperty::calcA(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& A){
    A = DIM2*DIM4-DIM1*(DIM3-DIM4);
}

void MAST::Solid1DZSectionProperty::calcdA(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dA){
    dA = -DIM1*dDIM3+DIM4*dDIM2-dDIM1*(DIM3-DIM4)+dDIM4*(DIM1+DIM2);
}

void MAST::Solid1DZSectionProperty::calcIz(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& Iz){
    Iz = (DIM2*(DIM4*DIM4*DIM4))/1.2E+1-(DIM1*(DIM3*DIM3*DIM3-DIM4*DIM4*DIM4))/1.2E+1;
}

void MAST::Solid1DZSectionProperty::calcdIz(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dIz){
    dIz = ((DIM4*DIM4*DIM4)*dDIM2)/1.2E+1-(dDIM1*(DIM3*DIM3*DIM3-DIM4*DIM4*DIM4))/1.2E+1+((DIM4*DIM4)*dDIM4*(DIM1+DIM2))/4.0-(DIM1*(DIM3*DIM3)*dDIM3)/4.0;
}

void MAST::Solid1DZSectionProperty::calcIy(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& Iy){
    Iy = ((DIM2*DIM2*DIM2)*DIM4)/1.2E+1-(DIM1*(DIM3-DIM4)*((DIM1*DIM1)*4.0+(DIM2*DIM2)*3.0+DIM1*DIM2*6.0))/1.2E+1;
}

void MAST::Solid1DZSectionProperty::calcdIy(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dIy){
    dIy = dDIM4*((DIM2*DIM2*DIM2)/1.2E+1+(DIM1*((DIM1*DIM1)*4.0+(DIM2*DIM2)*3.0+DIM1*DIM2*6.0))/1.2E+1)+dDIM2*(((DIM2*DIM2)*DIM4)/4.0-(DIM1*(DIM3-DIM4)*(DIM1*6.0+DIM2*6.0))/1.2E+1)-(DIM1*dDIM3*((DIM1*DIM1)*4.0+(DIM2*DIM2)*3.0+DIM1*DIM2*6.0))/1.2E+1-(dDIM1*pow(DIM1*2.0+DIM2,2.0)*(DIM3-DIM4))/4.0;
}

void MAST::Solid1DZSectionProperty::calcIp(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& Ip){
    Ip = (DIM2*(DIM4*DIM4*DIM4))/1.2E+1+((DIM2*DIM2*DIM2)*DIM4)/1.2E+1-(DIM1*(DIM3*DIM3*DIM3-DIM4*DIM4*DIM4))/1.2E+1-(DIM1*(DIM3-DIM4)*((DIM1*DIM1)*4.0+(DIM2*DIM2)*3.0+DIM1*DIM2*6.0))/1.2E+1;
}

void MAST::Solid1DZSectionProperty::calcdIp(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dIp){
    dIp = dDIM2*(((DIM2*DIM2)*DIM4)/4.0+(DIM4*DIM4*DIM4)/1.2E+1-(DIM1*(DIM3-DIM4)*(DIM1*6.0+DIM2*6.0))/1.2E+1)-(DIM1*dDIM3*((DIM1*DIM1)*4.0+(DIM2*DIM2)*3.0+(DIM3*DIM3)*3.0+DIM1*DIM2*6.0))/1.2E+1-(dDIM1*(DIM3-DIM4)*((DIM1*DIM1)*1.2E+1+(DIM2*DIM2)*3.0+DIM3*DIM3+DIM4*DIM4+DIM1*DIM2*1.2E+1+DIM3*DIM4))/1.2E+1+(dDIM4*(DIM1+DIM2)*((DIM1*DIM1)*4.0+DIM2*DIM2+(DIM4*DIM4)*3.0+DIM1*DIM2*2.0))/1.2E+1;
}

void MAST::Solid1DZSectionProperty::calcJ1_w(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& J1_w){
    J1_w = (DIM2*DIM2*DIM2)*DIM3;
}

void MAST::Solid1DZSectionProperty::calcdJ1_w(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dJ1_w){
    dJ1_w = (DIM2*DIM2*DIM2)*dDIM3+(DIM2*DIM2)*DIM3*dDIM2*3.0;
}

void MAST::Solid1DZSectionProperty::calcJ2_w(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& J2_w){
    J2_w = DIM2*(DIM3*DIM3*DIM3);
}

void MAST::Solid1DZSectionProperty::calcdJ2_w(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dJ2_w){
    dJ2_w = (DIM3*DIM3*DIM3)*dDIM2+DIM2*(DIM3*DIM3)*dDIM3*3.0;
}

void MAST::Solid1DZSectionProperty::calcJ1_f(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& J1_f){
    J1_f = pow(DIM3-DIM4,3.0)*(DIM1+DIM2)*(-1.0/8.0);
}

void MAST::Solid1DZSectionProperty::calcdJ1_f(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dJ1_f){
    dJ1_f = dDIM1*pow(DIM3-DIM4,3.0)*(-1.0/8.0)-(dDIM2*pow(DIM3-DIM4,3.0))/8.0-dDIM3*pow(DIM3-DIM4,2.0)*(DIM1+DIM2)*(3.0/8.0)+dDIM4*pow(DIM3-DIM4,2.0)*(DIM1+DIM2)*(3.0/8.0);
}

void MAST::Solid1DZSectionProperty::calcJ2_f(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& J2_f){
    J2_f = -(DIM3/2.0-DIM4/2.0)*pow(DIM1+DIM2,3.0);
}

void MAST::Solid1DZSectionProperty::calcdJ2_f(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dJ2_f){
    dJ2_f = dDIM3*pow(DIM1+DIM2,3.0)*(-1.0/2.0)+(dDIM4*pow(DIM1+DIM2,3.0))/2.0-dDIM1*(DIM3/2.0-DIM4/2.0)*pow(DIM1+DIM2,2.0)*3.0-dDIM2*(DIM3/2.0-DIM4/2.0)*pow(DIM1+DIM2,2.0)*3.0;
}

void MAST::Solid1DZSectionProperty::calck1_f(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& k1_f){
    k1_f = 1.0/3.0;
}

void MAST::Solid1DZSectionProperty::calcdk1_f(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dk1_f){
    dk1_f = 0.0;
}

void MAST::Solid1DZSectionProperty::calck2_f(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& k2_f){
    k2_f = 1.0/3.0;
}

void MAST::Solid1DZSectionProperty::calcdk2_f(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dk2_f){
    dk2_f = 0.0;
}

void MAST::Solid1DZSectionProperty::calck1_w(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& k1_w){
    k1_w = 1.0/3.0;
}

void MAST::Solid1DZSectionProperty::calcdk1_w(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dk1_w){
    dk1_w = 0.0;
}

void MAST::Solid1DZSectionProperty::calck2_w(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& k2_w){
    k2_w = 1.0/3.0;
}

void MAST::Solid1DZSectionProperty::calcdk2_w(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dk2_w){
    dk2_w = 0.0;
}

void MAST::Solid1DZSectionProperty::calcJc(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& Jc){
    Jc = pow(DIM3/2.0-DIM4/2.0,4.0)*(-2.1E+1/1.0E+2)-((DIM2*DIM2)*1.0/pow(DIM3/2.0-DIM4/2.0,2.0)*9.45E-2+(DIM2*2.620997766195086E-1)/(DIM3/2.0-DIM4/2.0)+9.08E-2)*pow(DIM2*2.0-DIM3+DIM4-sqrt(-DIM2*(DIM3-DIM4))*2.0,4.0)*2.0;
}

void MAST::Solid1DZSectionProperty::calcdJc(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dJc){
    dJc = -dDIM2*((2.620997766195086E-1/(DIM3/2.0-DIM4/2.0)+DIM2*1.0/pow(DIM3/2.0-DIM4/2.0,2.0)*(1.89E+2/1.0E+3))*pow(DIM2*2.0-DIM3+DIM4-sqrt(-DIM2*(DIM3-DIM4))*2.0,4.0)*2.0+((DIM3-DIM4)*1.0/sqrt(-DIM2*(DIM3-DIM4))+2.0)*((DIM2*DIM2)*1.0/pow(DIM3/2.0-DIM4/2.0,2.0)*9.45E-2+(DIM2*2.620997766195086E-1)/(DIM3/2.0-DIM4/2.0)+9.08E-2)*pow(DIM2*2.0-DIM3+DIM4-sqrt(-DIM2*(DIM3-DIM4))*2.0,3.0)*8.0)-dDIM3*(pow(DIM3/2.0-DIM4/2.0,3.0)*(2.1E+1/5.0E+1)-((DIM2*DIM2)*1.0/pow(DIM3/2.0-DIM4/2.0,3.0)*9.45E-2+DIM2*1.0/pow(DIM3/2.0-DIM4/2.0,2.0)*1.310498883097543E-1)*pow(DIM2*2.0-DIM3+DIM4-sqrt(-DIM2*(DIM3-DIM4))*2.0,4.0)*2.0+(DIM2*1.0/sqrt(-DIM2*(DIM3-DIM4))-1.0)*((DIM2*DIM2)*1.0/pow(DIM3/2.0-DIM4/2.0,2.0)*9.45E-2+(DIM2*2.620997766195086E-1)/(DIM3/2.0-DIM4/2.0)+9.08E-2)*pow(DIM2*2.0-DIM3+DIM4-sqrt(-DIM2*(DIM3-DIM4))*2.0,3.0)*8.0)+dDIM4*(pow(DIM3/2.0-DIM4/2.0,3.0)*(2.1E+1/5.0E+1)-((DIM2*DIM2)*1.0/pow(DIM3/2.0-DIM4/2.0,3.0)*9.45E-2+DIM2*1.0/pow(DIM3/2.0-DIM4/2.0,2.0)*1.310498883097543E-1)*pow(DIM2*2.0-DIM3+DIM4-sqrt(-DIM2*(DIM3-DIM4))*2.0,4.0)*2.0+(DIM2*1.0/sqrt(-DIM2*(DIM3-DIM4))-1.0)*((DIM2*DIM2)*1.0/pow(DIM3/2.0-DIM4/2.0,2.0)*9.45E-2+(DIM2*2.620997766195086E-1)/(DIM3/2.0-DIM4/2.0)+9.08E-2)*pow(DIM2*2.0-DIM3+DIM4-sqrt(-DIM2*(DIM3-DIM4))*2.0,3.0)*8.0);
}




// TODO: Improve accuracy of calculation of torsion constant and corresponding derivative
void MAST::Solid1DZSectionProperty::calcJ(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& J)
{
    Real t_f = (DIM4-DIM3)/2;
    Real w_f = DIM1+DIM2;
    Real h_w = DIM3;
    Real t_w = DIM2;
    
    Real k_f, k_w, c, J_w, J_f;
    Real wf = t_w/t_f;
    
    // Sum of rectangles with correction of El Darwish and Johnson
    if ( (wf>0.5) and (wf<1.0) )
    {
        k_f = 0.3333333333333333;
        k_w = 0.3333333333333333;
        calcJc(DIM1, DIM2, DIM3, DIM4, c);
    }
    
    // Sum of rectangles method, with rectangles corrected for aspect ratios
    else
    {
        if (w_f>t_f){
            calck1_f(DIM1, DIM2, DIM3, DIM4, k_f);
        }
        else{
            calck2_f(DIM1, DIM2, DIM3, DIM4, k_f);
        }
        
        if (h_w>t_w){
            calck1_w(DIM1, DIM2, DIM3, DIM4, k_w);
        }
        else{
            calck2_w(DIM1, DIM2, DIM3, DIM4, k_w);
        }
        c = 0.0;
    }
    
    
    if (w_f>t_f){
        calcJ1_f(DIM1, DIM2, DIM3, DIM4, J_f);
    }
    else{
        calcJ2_f(DIM1, DIM2, DIM3, DIM4, J_f);
    }
    
    if (h_w>t_w){
        calcJ1_w(DIM1, DIM2, DIM3, DIM4, J_w);
    }
    else{
        calcJ2_w(DIM1, DIM2, DIM3, DIM4, J_w);
    }
    
    J = 2.0*k_f*J_f + k_w*J_w + c;
}



void MAST::Solid1DZSectionProperty::calcdJ(Real& DIM1, Real& DIM2, Real& DIM3, Real& DIM4, Real& dDIM1, Real& dDIM2, Real& dDIM3, Real& dDIM4, Real& dJ)
{
    Real t_f = (DIM4-DIM3)/2;
    Real w_f = DIM1+DIM2;
    Real h_w = DIM3;
    Real t_w = DIM2;
    
    Real k_f, k_w, c, dk_f, dk_w, dc, J_w, J_f, dJ_w, dJ_f;
    Real wf = t_w/t_f;
    
    // Sum of rectangles with correction of El Darwish and Johnson
    if ( (wf>0.5) and (wf<1.0) )
    {
        k_f = 0.3333333333333333;
        k_w = 0.3333333333333333;
        dk_f = 0.0;
        dk_w = 0.0;
        calcdJc(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dc);
    }
    
    // Sum of rectangles method, with rectangles corrected for aspect ratios
    else
    {
        if (w_f>t_f){
            calck1_f(DIM1, DIM2, DIM3, DIM4, k_f);
            calcdk1_f(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dk_f);
        }
        else{
            calck2_f(DIM1, DIM2, DIM3, DIM4, k_f);
            calcdk2_f(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dk_f);
        }
        
        if (h_w>t_w){
            calck1_w(DIM1, DIM2, DIM3, DIM4, k_w);
            calcdk1_w(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dk_w);
        }
        else{
            calck2_w(DIM1, DIM2, DIM3, DIM4, k_w);
            calcdk2_w(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dk_w);
        }
        dc = 0.0;
    }
    
    
    if (w_f>t_f){
        calcJ1_f(DIM1, DIM2, DIM3, DIM4, J_f);
        calcdJ1_f(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dJ_f);
    }
    else{
        calcJ2_f(DIM1, DIM2, DIM3, DIM4, J_f);
        calcdJ2_f(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dJ_f);
    }
    
    if (h_w>t_w){
        calcJ1_w(DIM1, DIM2, DIM3, DIM4, J_w);
        calcdJ1_w(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dJ_w);
    }
    else{
        calcJ2_w(DIM1, DIM2, DIM3, DIM4, J_w);
        calcdJ2_w(DIM1, DIM2, DIM3, DIM4, dDIM1, dDIM2, dDIM3, dDIM4, dJ_w);
    }
    
    dJ = 2.0*(dk_f*J_f + k_f*dJ_f) + dk_w*J_w + k_w*dJ_w + dc;
}


void MAST::Solid1DZSectionElementPropertyCard::init() {
    
    libmesh_assert(!_initialized);
    
    MAST::FieldFunction<Real>
    &DIM1     =  this->get<MAST::FieldFunction<Real> >("DIM1"),
    &DIM2     =  this->get<MAST::FieldFunction<Real> >("DIM2"),
    &DIM3     =  this->get<MAST::FieldFunction<Real> >("DIM3"),
    &DIM4     =  this->get<MAST::FieldFunction<Real> >("DIM4"),
    &hy_off =  this->get<MAST::FieldFunction<Real> >("hy_off"),
    &hz_off =  this->get<MAST::FieldFunction<Real> >("hz_off");
    
    // Check that dimensions are physically correct
    Real DIM1v, DIM2v, DIM3v, DIM4v, DIM5v;
    DIM1(DIM1v); DIM2(DIM2v); DIM3(DIM3v); DIM4(DIM4v);
    if (DIM1v<=0){
        libmesh_error_msg("DIM1<=0");
    }
    else if (DIM2v<=0){
        libmesh_error_msg("DIM2<=0");
    }
    else if (DIM3v<=0){
        libmesh_error_msg("DIM3<=0");
    }
    else if (DIM4v<=0){
        libmesh_error_msg("DIM4<=0");
    }
    else if (DIM4v<=DIM3v){
        libmesh_error_msg("DIM4<=DIM3");
    }

    _A.reset(new MAST::Solid1D4ParameterSectionProperty::Area(MAST::Solid1DZSectionProperty::calcA,
                                                              MAST::Solid1DZSectionProperty::calcdA,
                                                              DIM1, DIM2, DIM3, 
                                                              DIM4));
    
    _Ay.reset(new MAST::Solid1D4ParameterSectionProperty::AreaYMoment(
                                                                MAST::Solid1DZSectionProperty::calcA,
                                                                MAST::Solid1DZSectionProperty::calcdA,
                                                                DIM1, DIM2, 
                                                                DIM3, DIM4,
                                                                hz_off));
    
    _Az.reset(new MAST::Solid1D4ParameterSectionProperty::AreaZMoment(
                                                                MAST::Solid1DZSectionProperty::calcA,
                                                                MAST::Solid1DZSectionProperty::calcdA,
                                                                DIM1, DIM2, 
                                                                DIM3, DIM4,
                                                                hy_off));
    
    _J.reset(new MAST::Solid1D4ParameterSectionProperty::TorsionalConstant(
                                                                MAST::Solid1DZSectionProperty::calcJ,
                                                                MAST::Solid1DZSectionProperty::calcdJ,
                                                                DIM1, DIM2,
                                                                DIM3, DIM4));
    
    _Ip.reset(new MAST::Solid1D4ParameterSectionProperty::PolarInertia(
                                                                MAST::Solid1DZSectionProperty::calcIp,
                                                                MAST::Solid1DZSectionProperty::calcdIp,
                                                                MAST::Solid1DZSectionProperty::calcA,
                                                                MAST::Solid1DZSectionProperty::calcdA,
                                                                DIM1, DIM2,
                                                                DIM3, DIM4,
                                                                hy_off,
                                                                hz_off));
    
    _AI.reset(new MAST::Solid1D4ParameterSectionProperty::AreaInertiaMatrix(
                                                                MAST::Solid1DZSectionProperty::calcIz,
                                                                MAST::Solid1DZSectionProperty::calcdIz,
                                                                MAST::Solid1DZSectionProperty::calcIy,
                                                                MAST::Solid1DZSectionProperty::calcdIy,
                                                                MAST::Solid1DZSectionProperty::calcA,
                                                                MAST::Solid1DZSectionProperty::calcdA,
                                                                DIM1, DIM2,
                                                                DIM3, DIM4,
                                                                hy_off,
                                                                hz_off));
    
    _initialized = true;
}