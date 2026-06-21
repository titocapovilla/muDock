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

    C3 = 0, // C3

    C3x = 1, // C3x

    C3un = 2, // C3un

    C2 = 3, // C2

    C2x = 4, // C2x

    C2un = 5, // C2un

    Car = 6, // Car

    Carx = 7, // Carx

    Carun = 8, // Carun

    C1 = 9, // C1

    C1x = 10, // C1x

    C1un = 11, // C1un

    Ccat = 12, // Ccat

    N3h = 13, // N3h

    N3 = 14, // N3

    N3un = 15, // N3un

    Npl3h = 16, // Npl3h

    Npl3 = 17, // Npl3

    Npl3un = 18, // Npl3un

    N2h = 19, // N2h

    N2 = 20, // N2

    N2un = 21, // N2un

    Narh = 22, // Narh

    Nar = 23, // Nar

    Narun = 24, // Narun

    N1 = 25, // N1

    N1un = 26, // N1un

    N4 = 27, // N4

    O3h = 28, // O3h

    O3 = 29, // O3

    O3un = 30, // O3un

    O2 = 31, // O2

    O2un = 32, // O2un

    Oco2 = 33, // Oco2

    S3h = 34, // S3h

    S3 = 35, // S3

    S3un = 36, // S3un

    S2 = 37, // S2

    S2un = 38, // S2un

    So = 39, // So

    P3 = 40, // P3

    F = 41, // F

    Cl = 42, // Cl

    Br = 43, // Br

    I = 44, // I

    H = 45, // H

    Hhb = 46, // Hhb

    Si = 47, // Si

    Ow = 48, // Ow

    Mplus = 49, // Mplus

    Un = 50, // Un

    // non xtool types added for type mismatch in residue (added manually)

    //xlogp
    Nam = 51, // Nam

    So2 = 52, // So2

    //ions
    Li = 53, // Li

    Na = 54, // Na

    K = 55, // K

    Mg = 56, // Mg

    Ca = 57, // Ca

    Mn = 58, // Mn

    Fe = 59, // Fe

    Co = 60, // Co

    Ni = 61, // Ni

    Cu = 62, // Cu

    Zn = 63, // Zn

    Cd = 64, // Cd

    Hg = 65, // Hg

    Al = 66, // Al

    U = 67, // U

    Fminus = 68, // Fminus

    Clminus = 69, // Clminus

    Brminus = 70, // Brminus

    Iminus = 71, // Iminus

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
    static constexpr auto num_xtool_ff() { return 72; }
    extern const std::array<xtool_ff_description, num_xtool_ff()> XTOOL_FF_DICTIONARY;


  // Utility function to get the description
  inline const xtool_ff_description& get_description(const xtool_ff a) {
    assert(XTOOL_FF_DICTIONARY[static_cast<int>(a)].value == a);
    return XTOOL_FF_DICTIONARY[static_cast<int>(a)];
  }

  xtool_ff parse_xtool_type(const std::string_view symbol);
} // namespace mudock