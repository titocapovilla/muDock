#pragma once

#include <array>
#include <cassert>
#include <mudock/chem/elements.hpp>
#include <mudock/type_alias.hpp>
#include <string_view>

//===------------------------------------------------------------------------------------------------------
// WARNING: This file has been automatically generated from chem/autodock_babel_types.json
//===------------------------------------------------------------------------------------------------------

namespace mudock {

  // this is the list of all the known atoms
  enum class xtool_atom_type : int {
    C_3      = 0,  // C.3       carbon sp3 non-polar 
    C_3_x    = 1,  // C.3.x     carbon sp3 polar
    C_3_un   = 2,  // C.3.un    carbon sp3 unknown 
    C_2      = 3,  // C.2       carbon sp2 non-polar 
    C_2_x    = 4,  // C.2.x     carbon sp2 polar 
    C_2_un   = 5,  // C.2.un    carbon sp2 unknown 
    C_ar     = 6,  // C.ar      carbon aromatic non-polar
    C_ar_x   = 7,  // C.ar.x    carbon aromatic polar
    C_ar_un  = 8,  // C.ar.un   carbon aromatic unknown 
    C_1      = 9,  // C.1       carbon sp non-polar
    C_1_x    = 10, // C.1.x     carbon sp polar
    C_1_un   = 11, // C.1.un    carbon sp unknown
    C_cat    = 12, // C.cat     carboncation (C+)
    N_3_h    = 13, // N.3.h     nitrogen sp3 with H 
    N_3      = 14, // N.3       nitrogen sp3  
    N_3_un   = 15, // N.3.un    nitrogen sp3 unknown
    N_pl3_h  = 16, // N.pl3.h   nitrogen sp3 planar with H
    N_pl3    = 17, // N.pl3     nitrogen sp3 planar 
    N_pl3_un = 18, // N.pl3.un  nitrogen sp3 planar unknown
    N_2_h    = 19, // N.2.h     nitrogen sp2 with H
    N_2      = 20, // N.2       nitrogen sp2 
    N_2_un   = 21, // N.2.un    nitrogen sp2 unknown
    N_ar_h   = 22, // N.ar.h    nitrogen aromatic with H
    N_ar     = 23, // N.ar      nitrogen aromatic 
    N_ar_un  = 24, // N.ar.un   nitrogen aromatic unknown 
    N_1      = 25, // N.1       nitrogen sp
    N_1_un   = 26, // N.1.un    nitrogen sp unknown
    N_4      = 27, // N.4       nitrogen sp3 charged (N+)
    O_3_h    = 28, // O.3.h     oxygen sp3 with H
    O_3      = 29, // O.3       oxygen sp3
    O_3_un   = 30, // O.3.un    oxygen sp3 unknown
    O_2      = 31, // O.2       oxygen sp2
    O_2_un   = 32, // O.2.un    oxygen sp2 unknown
    O_co2    = 33, // O.co2     oxygen in carboxylate 
    S_3_h    = 34, // S.3.h     sulfur sp3 with H
    S_3      = 35, // S.3       sulfur sp3
    S_3_un   = 36, // S.3.un    sulfur sp3 unknown
    S_2      = 37, // S.2       sulfur sp2 	
    S_2_un   = 38, // S.2.un    sulfur sp2 unknown 	
    S_o      = 39, // S.o       sulfur in sulfoxide or sulfone 
    P_3      = 40, // P.3       phosphorous sp3
    F        = 41, // F         fluorine
    Cl       = 42, // Cl        chlorine
    Br       = 43, // Br        bromine
    I        = 44, // I         iodine
    H        = 45, // H         hydrogen non-polar
    H_hb     = 46, // H.hb      hydrogen polar
    Si       = 47, // Si        silicon sp3 
    O_w      = 48, // O.w       water 
    M_plus   = 49, // M+        metal ions 
    Un       = 50  // Un        unknown atom 
  };

  enum class xlogp_atom_type : int {
    C_3_h3_pi_eq_0     = 0,  // C.3.h3.pi=0      
    C_3_h3_pi_eq_1     = 1,  // C.3.h3.pi=1      
    C_3_h3_x           = 2,  // C.3.h3.x         
    C_3_h2_pi_eq_0     = 3,  // C.3.h2.pi=0      
    C_3_h2_pi_eq_1     = 4,  // C.3.h2.pi=1      
    C_3_h2_pi_eq_2     = 5,  // C.3.h2.pi=2      
    C_3_h2_x_pi_eq_0   = 6,  // C.3.h2.x.pi=0    
    C_3_h2_x_pi_eq_1   = 7,  // C.3.h2.x.pi=1    
    C_3_h2_x_pi_eq_2   = 8,  // C.3.h2.x.pi=2    
    C_3_h_pi_eq_0      = 9,  // C.3.h.pi=0       
    C_3_h_pi_eq_1      = 10, // C.3.h.pi=1       
    C_3_h_pi_gt_1      = 11, // C.3.h.pi>1       
    C_3_h_x_pi_eq_0    = 12, // C.3.h.x.pi=0     
    C_3_h_x_pi_eq_1    = 13, // C.3.h.x.pi=1     
    C_3_h_x_pi_gt_1    = 14, // C.3.h.x.pi>1     
    C_3_pi_eq_0        = 15, // C.3.pi=0         
    C_3_pi_eq_1        = 16, // C.3.pi=1         
    C_3_pi_gt_1        = 17, // C.3.pi>1         
    C_3_x_pi_eq_0      = 18, // C.3.x.pi=0       
    C_3_x_pi_gt_0      = 19, // C.3.x.pi>0       
    C_3_unknown        = 20, // C.3.unknown      
    C_2_h2             = 21, // C.2.h2           
    C_2_h_pi_eq_0      = 22, // C.2.h.pi=0       
    C_2_h_pi_eq_1      = 23, // C.2.h.pi=1       
    C_2_h_x_pi_eq_0    = 24, // C.2.h.x.pi=0     
    C_2_h_x_pi_eq_1    = 25, // C.2.h.x.pi=1     
    C_2_pi_eq_0        = 26, // C.2.pi=0         
    C_2_pi_gt_0        = 27, // C.2.pi>0         
    C_2_x_pi_eq_0      = 28, // C.2.x.pi=0       
    C_2_x_pi_gt_0      = 29, // C.2.x.pi>0       
    C_2_x2_pi_eq_0     = 30, // C.2.x2.pi=0      
    C_2_x2_pi_gt_0     = 31, // C.2.x2.pi>0      
    C_2_unknown        = 32, // C.2.unknown      
    C_ar_h             = 33, // C.ar.h           
    C_ar_h_X           = 34, // C.ar.h.(X)       
    C_ar               = 35, // C.ar             
    C_ar_x             = 36, // C.ar.x           
    C_ar_X             = 37, // C.ar.(X)         
    C_ar_X_x           = 38, // C.ar.(X).x       
    C_ar_unknown       = 39, // C.ar.unknown     
    C_1_h              = 40, // C.1.h            
    C_1                = 41, // C.1              
    C_1_eq_eq          = 42, // C.1.==           
    C_1_unknown        = 43, // C.1.unknown      
    C_cat              = 44, // C.cat            
    N_3_h2_pi_eq_0     = 45, // N.3.h2.pi=0      
    N_3_h2_pi_eq_1     = 46, // N.3.h2.pi=1      
    N_3_h2_x           = 47, // N.3.h2.x         
    N_3_h_pi_eq_0      = 48, // N.3.h.pi=0       
    N_3_h_pi_gt_0      = 49, // N.3.h.pi>0       
    N_3_h_ring         = 50, // N.3.h.ring       
    N_3_h_x            = 51, // N.3.h.x          
    N_3_h_x_ring       = 52, // N.3.h.x.ring     
    N_3_pi_eq_0        = 53, // N.3.pi=0         
    N_3_pi_gt_0        = 54, // N.3.pi>0         
    N_3_ring           = 55, // N.3.ring         
    N_3_x              = 56, // N.3.x            
    N_3_x_ring         = 57, // N.3.x.ring       
    N_3_unknown        = 58, // N.3.unknown      
    N_am_h2            = 59, // N.am.h2          
    N_am_h             = 60, // N.am.h           
    N_am_h_x           = 61, // N.am.h.x         
    N_am               = 62, // N.am             
    N_am_x             = 63, // N.am.x           
    N_am_unknown       = 64, // N.am.unknown     
    N_2_eq_C_pi_eq_0   = 65, // N.2.(=C).pi=0    
    N_2_eq_C_pi_eq_1   = 66, // N.2.(=C).pi=1    
    N_2_eq_C_x_pi_eq_0 = 67, // N.2.(=C).x.pi=0  
    N_2_eq_C_x_pi_eq_1 = 68, // N.2.(=C).x.pi=1  
    N_2_eq_N           = 69, // N.2.(=N)         
    N_2_eq_N_x         = 70, // N.2.(=N).x       
    N_2_o              = 71, // N.2.o            
    N_2_o2             = 72, // N.2.o2           
    N_2_unknown        = 73, // N.2.unknown      
    N_ar               = 74, // N.ar             
    N_1                = 75, // N.1              
    N_4                = 76, // N.4              
    O_3_h_pi_eq_0      = 77, // O.3.h.pi=0       
    O_3_h_pi_eq_1      = 78, // O.3.h.pi=1       
    O_3_h_x            = 79, // O.3.h.x          
    O_3_pi_eq_0        = 80, // O.3.pi=0         
    O_3_pi_gt_0        = 81, // O.3.pi>0         
    O_3_x              = 82, // O.3.x            
    O_3_unknown        = 83, // O.3.unknown      
    O_2                = 84, // O.2              
    O_co2              = 85, // O.co2            
    S_3_h              = 86, // S.3.h            
    S_3                = 87, // S.3              
    S_3_unknown        = 88, // S.3.unknown      
    S_2                = 89, // S.2              
    S_o                = 90, // S.o              
    S_o2               = 91, // S.o2             
    P_3_eq_O           = 92, // P.3.(=O)         
    P_3_eq_S           = 93, // P.3.(=S)         
    P_3_unknown        = 94, // P.3.unknown      
    F_pi_eq_0          = 95, // F.pi=0           
    F_pi_eq_1          = 96, // F.pi=1           
    F_unknown          = 97, // F.unknown        
    Cl_pi_eq_0         = 98, // Cl.pi=0          
    Cl_pi_eq_1         = 99, // Cl.pi=1          
    Cl_unknown         = 100, // Cl.unknown       
    Br_pi_eq_0         = 101, // Br.pi=0          
    Br_pi_eq_1         = 102, // Br.pi=1          
    Br_unknown         = 103, // Br.unknown       
    I_pi_eq_0          = 104, // I.pi=0           
    I_pi_eq_1          = 105, // I.pi=1           
    I_unknown          = 106, // I.unknown        
    H                  = 107, // H                
    H_hb               = 108, // H.hb             
    Si                 = 109, // Si               
    Un                 = 110, // Un               
    Du                 = 111  // Du               
  };

  enum class hb_character : int {
    NONE        = 0, // 'N'
    DONOR       = 1, // 'D'
    ACCEPTOR    = 2, // 'A'
    BOTH        = 3, // 'DA'
    POLAR_H     = 4, // 'DH'
    POLAR       = 5, // 'P'
    HYDROPHOBIC = 6, // 'H'
    METAL       = 7  // 'M'
  };

    struct x_score_ff_description {
    //instanciate variables to fill with
    // # The 1st column: atom type number 
    // # The 2nd column: X-Tool atom type 
    // # The 3rd column: atomic weight
    // # The 4th column: X-Tool van der Waals radius
    // # The 5th column: X-Tool van der Waals potential
    // # The 6th column: X-Tool atomic partial charge
    // # The 7th column: hydrogen bonding character
  };

} // namespace mudock
