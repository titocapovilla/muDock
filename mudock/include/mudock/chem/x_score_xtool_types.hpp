#pragma once

#include <array>
#include <cassert>
#include <mudock/type_alias.hpp>
#include <string_view>

//===------------------------------------------------------------------------------------------------------
// WARNING: This file has been automatically generated from chem/x_score_xtool_types.json
//===------------------------------------------------------------------------------------------------------

namespace mudock {

  // List of all known atoms for X-TOOL force field
  enum class xtool_ff : int {

    C_3 = 0, // C_3

    C_3_x = 1, // C_3_x

    C_3_un = 2, // C_3_un

    C_2 = 3, // C_2

    C_2_x = 4, // C_2_x

    C_2_un = 5, // C_2_un

    C_ar = 6, // C_ar

    C_ar_x = 7, // C_ar_x

    C_ar_un = 8, // C_ar_un

    C_1 = 9, // C_1

    C_1_x = 10, // C_1_x

    C_1_un = 11, // C_1_un

    C_cat = 12, // C_cat

    N_3_h = 13, // N_3_h

    N_3 = 14, // N_3

    N_3_un = 15, // N_3_un

    N_pl3_h = 16, // N_pl3_h

    N_pl3 = 17, // N_pl3

    N_pl3_un = 18, // N_pl3_un

    N_2_h = 19, // N_2_h

    N_2 = 20, // N_2

    N_2_un = 21, // N_2_un

    N_ar_h = 22, // N_ar_h

    N_ar = 23, // N_ar

    N_ar_un = 24, // N_ar_un

    N_1 = 25, // N_1

    N_1_un = 26, // N_1_un

    N_4 = 27, // N_4

    O_3_h = 28, // O_3_h

    O_3 = 29, // O_3

    O_3_un = 30, // O_3_un

    O_2 = 31, // O_2

    O_2_un = 32, // O_2_un

    O_co2 = 33, // O_co2

    S_3_h = 34, // S_3_h

    S_3 = 35, // S_3

    S_3_un = 36, // S_3_un

    S_2 = 37, // S_2

    S_2_un = 38, // S_2_un

    S_o = 39, // S_o

    P_3 = 40, // P_3

    F = 41, // F

    Cl = 42, // Cl

    Br = 43, // Br

    I = 44, // I

    H = 45, // H

    H_hb = 46, // H_hb

    Si = 47, // Si

    O_w = 48, // O_w

    Mplus = 49, // Mplus

    Un = 50, // Un

  };

  // Knowledge about the X-TOOL force field parameters
  struct xtool_ff_description {
    xtool_ff value;
    std::string_view name;
    fp_type atomic_weight;
    fp_type vdw_radius;
    fp_type vdw_potential;
    fp_type par_charge;
    std::string_view hbond;
  };
  static constexpr auto num_xtool_ff() { return 51; }
  extern const std::array<xtool_ff_description, num_xtool_ff()> XTOOL_FF_DICTIONARY;

  // Utility function to get the description
  inline const xtool_ff_description& get_description(const xtool_ff a) {
    assert(XTOOL_FF_DICTIONARY[static_cast<int>(a)].value == a);
    return XTOOL_FF_DICTIONARY[static_cast<int>(a)];
  }

  xtool_ff parse_xtool_type(const std::string_view symbol);
} // namespace mudock