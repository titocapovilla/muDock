#pragma once

#include <array>
#include <cassert>
#include <mudock/chem/x_score_hb.hpp>
#include <mudock/type_alias.hpp>
#include <string_view>

//===------------------------------------------------------------------------------------------------------
// WARNING: This file has been automatically generated from chem/x_score_xlogp_types.json
//===------------------------------------------------------------------------------------------------------

namespace mudock {

  // List of all known atoms for X-LOGP force field
  enum class xlogp_ff : int {

    C_3_h3_pi_eq_0 = 0, // C_3_h3_pi_eq_0
  
    C_3_h3_pi_eq_1 = 1, // C_3_h3_pi_eq_1

    C_3_h3_x = 2, // C_3_h3_x

    C_3_h2_pi_eq_0 = 3, // C_3_h2_pi_eq_0

    C_3_h2_pi_eq_1 = 4, // C_3_h2_pi_eq_1

    C_3_h2_pi_eq_2 = 5, // C_3_h2_pi_eq_2

    C_3_h2_x_pi_eq_0 = 6, // C_3_h2_x_pi_eq_0

    C_3_h2_x_pi_eq_1 = 7, // C_3_h2_x_pi_eq_1

    C_3_h2_x_pi_eq_2 = 8, // C_3_h2_x_pi_eq_2

    C_3_h_pi_eq_0 = 9, // C_3_h_pi_eq_0

    C_3_h_pi_eq_1 = 10, // C_3_h_pi_eq_1

    C_3_h_pi_gt_1 = 11, // C_3_h_pi_gt_1

    C_3_h_x_pi_eq_0 = 12, // C_3_h_x_pi_eq_0

    C_3_h_x_pi_eq_1 = 13, // C_3_h_x_pi_eq_1

    C_3_h_x_pi_gt_1 = 14, // C_3_h_x_pi_gt_1

    C_3_pi_eq_0 = 15, // C_3_pi_eq_0

    C_3_pi_eq_1 = 16, // C_3_pi_eq_1

    C_3_pi_gt_1 = 17, // C_3_pi_gt_1

    C_3_x_pi_eq_0 = 18, // C_3_x_pi_eq_0

    C_3_x_pi_gt_0 = 19, // C_3_x_pi_gt_0

    C_3_unknown = 20, // C_3_unknown

    C_2_h2 = 21, // C_2_h2

    C_2_h_pi_eq_0 = 22, // C_2_h_pi_eq_0

    C_2_h_pi_eq_1 = 23, // C_2_h_pi_eq_1

    C_2_h_x_pi_eq_0 = 24, // C_2_h_x_pi_eq_0

    C_2_h_x_pi_eq_1 = 25, // C_2_h_x_pi_eq_1

    C_2_pi_eq_0 = 26, // C_2_pi_eq_0

    C_2_pi_gt_0 = 27, // C_2_pi_gt_0

    C_2_x_pi_eq_0 = 28, // C_2_x_pi_eq_0

    C_2_x_pi_gt_0 = 29, // C_2_x_pi_gt_0

    C_2_x2_pi_eq_0 = 30, // C_2_x2_pi_eq_0

    C_2_x2_pi_gt_0 = 31, // C_2_x2_pi_gt_0

    C_2_unknown = 32, // C_2_unknown

    C_ar_h = 33, // C_ar_h

    C_ar_h_X = 34, // C_ar_h_X

    C_ar = 35, // C_ar

    C_ar_x = 36, // C_ar_x

    C_ar_X = 37, // C_ar_X

    C_ar_X_x = 38, // C_ar_X_x

    C_ar_unknown = 39, // C_ar_unknown

    C_1_h = 40, // C_1_h

    C_1 = 41, // C_1

    C_1_eq_eq = 42, // C_1_eq_eq

    C_1_unknown = 43, // C_1_unknown

    C_cat = 44, // C_cat

    N_3_h2_pi_eq_0 = 45, // N_3_h2_pi_eq_0

    N_3_h2_pi_eq_1 = 46, // N_3_h2_pi_eq_1

    N_3_h2_x = 47, // N_3_h2_x

    N_3_h_pi_eq_0 = 48, // N_3_h_pi_eq_0

    N_3_h_pi_gt_0 = 49, // N_3_h_pi_gt_0

    N_3_h_ring = 50, // N_3_h_ring

    N_3_h_x = 51, // N_3_h_x

    N_3_h_x_ring = 52, // N_3_h_x_ring

    N_3_pi_eq_0 = 53, // N_3_pi_eq_0

    N_3_pi_gt_0 = 54, // N_3_pi_gt_0

    N_3_ring = 55, // N_3_ring

    N_3_x = 56, // N_3_x

    N_3_x_ring = 57, // N_3_x_ring

    N_3_unknown = 58, // N_3_unknown

    N_am_h2 = 59, // N_am_h2

    N_am_h = 60, // N_am_h

    N_am_h_x = 61, // N_am_h_x

    N_am = 62, // N_am

    N_am_x = 63, // N_am_x

    N_am_unknown = 64, // N_am_unknown

    N_2_eq_C_pi_eq_0 = 65, // N_2_eq_C_pi_eq_0

    N_2_eq_C_pi_eq_1 = 66, // N_2_eq_C_pi_eq_1

    N_2_eq_C_x_pi_eq_0 = 67, // N_2_eq_C_x_pi_eq_0

    N_2_eq_C_x_pi_eq_1 = 68, // N_2_eq_C_x_pi_eq_1

    N_2_eq_N = 69, // N_2_eq_N

    N_2_eq_N_x = 70, // N_2_eq_N_x

    N_2_o = 71, // N_2_o

    N_2_o2 = 72, // N_2_o2

    N_2_unknown = 73, // N_2_unknown

    N_ar = 74, // N_ar

    N_1 = 75, // N_1

    N_4 = 76, // N_4

    O_3_h_pi_eq_0 = 77, // O_3_h_pi_eq_0

    O_3_h_pi_eq_1 = 78, // O_3_h_pi_eq_1

    O_3_h_x = 79, // O_3_h_x

    O_3_pi_eq_0 = 80, // O_3_pi_eq_0

    O_3_pi_gt_0 = 81, // O_3_pi_gt_0

    O_3_x = 82, // O_3_x

    O_3_unknown = 83, // O_3_unknown

    O_2 = 84, // O_2

    O_co2 = 85, // O_co2

    S_3_h = 86, // S_3_h

    S_3 = 87, // S_3

    S_3_unknown = 88, // S_3_unknown

    S_2 = 89, // S_2

    S_o = 90, // S_o

    S_o2 = 91, // S_o2

    P_3_eq_O = 92, // P_3_eq_O

    P_3_eq_S = 93, // P_3_eq_S

    P_3_unknown = 94, // P_3_unknown

    F_pi_eq_0 = 95, // F_pi_eq_0

    F_pi_eq_1 = 96, // F_pi_eq_1

    F_unknown = 97, // F_unknown

    Cl_pi_eq_0 = 98, // Cl_pi_eq_0

    Cl_pi_eq_1 = 99, // Cl_pi_eq_1

    Cl_unknown = 100, // Cl_unknown

    Br_pi_eq_0 = 101, // Br_pi_eq_0

    Br_pi_eq_1 = 102, // Br_pi_eq_1

    Br_unknown = 103, // Br_unknown

    I_pi_eq_0 = 104, // I_pi_eq_0

    I_pi_eq_1 = 105, // I_pi_eq_1

    I_unknown = 106, // I_unknown

    H = 107, // H

    H_hb = 108, // H_hb

    Si = 109, // Si

    Un = 110, // Un

    Du = 111, // Du

  };

  // Knowledge about the X-LOGP force field parameters
  struct xlogp_ff_description {
    xlogp_ff value;
    std::string_view name;
    x_score_hb hbond;
    fp_type hydrophobic_scale;
  };
  static constexpr auto num_xlogp_ff() { return 112; }
  extern const std::array<xlogp_ff_description, num_xlogp_ff()> XLOGP_FF_DICTIONARY;

  // Utility function to get the description
  inline const xlogp_ff_description& get_description(const xlogp_ff a) {
    assert(XLOGP_FF_DICTIONARY[static_cast<int>(a)].value == a);
    return XLOGP_FF_DICTIONARY[static_cast<int>(a)];
  }

  xlogp_ff parse_xlogp_type(const std::string_view symbol);


} // namespace mudock