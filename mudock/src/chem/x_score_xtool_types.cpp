#include <algorithm>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <stdexcept>

//===------------------------------------------------------------------------------------------------------
// WARNING: This file has been automatically generated from chem/xtool_types.json
//===------------------------------------------------------------------------------------------------------

namespace mudock {
  xtool_ff parse_xtool_type(const std::string_view symbol) {
    const auto element_it = std::find_if(std::begin(XTOOL_FF_DICTIONARY),
                                         std::end(XTOOL_FF_DICTIONARY),
                                         [&symbol](const auto& e) { return e.name == symbol; });
    if (element_it != std::end(XTOOL_FF_DICTIONARY))
      return element_it->value;
    else
      throw std::runtime_error("Missing xtool type");
  }
  const std::array<xtool_ff_description, 72> XTOOL_FF_DICTIONARY = {{
    {
      xtool_ff::C3,
      "C3",
      12.01,
      2.100,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::C3x,
      "C3x",
      12.01,
      2.100,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C3un,
      "C3un",
      12.01,
      2.100,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C2,
      "C2",
      12.01,
      1.900,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::C2x,
      "C2x",
      12.01,
      1.900,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C2un,
      "C2un",
      12.01,
      1.900,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::Car,
      "Car",
      12.01,
      2.000,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::Carx,
      "Carx",
      12.01,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::Carun,
      "Carun",
      12.01,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C1,
      "C1",
      12.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C1x,
      "C1x",
      12.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C1un,
      "C1un",
      12.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::Ccat,
      "Ccat",
      12.01,
      1.900,
      0.000,
      1.000,
      "P"
    },

    {
      xtool_ff::N3h,
      "N3h",
      14.01,
      1.800,
      0.000,
      0.000,
      "D"
    },

    {
      xtool_ff::N3,
      "N3",
      14.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N3un,
      "N3un",
      14.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::Npl3h,
      "Npl3h",
      14.01,
      1.750,
      0.000,
      0.000,
      "D"
    },

    {
      xtool_ff::Npl3,
      "Npl3",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::Npl3un,
      "Npl3un",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N2h,
      "N2h",
      14.01,
      1.750,
      0.000,
      0.000,
      "DA"
    },

    {
      xtool_ff::N2,
      "N2",
      14.01,
      1.750,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::N2un,
      "N2un",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::Narh,
      "Narh",
      14.01,
      1.750,
      0.000,
      0.000,
      "D"
    },

    {
      xtool_ff::Nar,
      "Nar",
      14.01,
      1.750,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::Narun,
      "Narun",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N1,
      "N1",
      14.01,
      1.750,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::N1un,
      "N1un",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N4,
      "N4",
      14.01,
      1.800,
      0.000,
      1.000,
      "D"
    },

    {
      xtool_ff::O3h,
      "O3h",
      16.00,
      1.650,
      0.000,
      0.000,
      "DA"
    },

    {
      xtool_ff::O3,
      "O3",
      16.00,
      1.650,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::O3un,
      "O3un",
      16.00,
      1.650,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::O2,
      "O2",
      16.00,
      1.550,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::O2un,
      "O2un",
      16.00,
      1.550,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::Oco2,
      "Oco2",
      16.00,
      1.550,
      0.000,
      -0.500,
      "DA"
    },

    {
      xtool_ff::S3h,
      "S3h",
      32.07,
      2.100,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::S3,
      "S3",
      32.07,
      2.100,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::S3un,
      "S3un",
      32.07,
      2.100,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::S2,
      "S2",
      32.07,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::S2un,
      "S2un",
      32.07,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::So,
      "So",
      32.07,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::P3,
      "P3",
      30.97,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::F,
      "F",
      19.00,
      1.500,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::Cl,
      "Cl",
      35.45,
      1.750,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::Br,
      "Br",
      79.90,
      1.900,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::I,
      "I",
      126.90,
      2.050,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::H,
      "H",
      1.00,
      1.000,
      0.000,
      0.000,
      "N"
    },

    {
      xtool_ff::Hhb,
      "Hhb",
      1.00,
      1.000,
      0.000,
      0.000,
      "DH"
    },

    {
      xtool_ff::Si,
      "Si",
      28.09,
      2.000,
      0.000,
      0.000,
      "N"
    },

    {
      xtool_ff::Ow,
      "Ow",
      16.00,
      1.750,
      0.000,
      0.000,
      "DA"
    },

    {
      xtool_ff::Mplus,
      "Mplus",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Un,
      "Un",
      0.00,
      0.000,
      0.000,
      0.000,
      "N"
    },

    // non xtool types added for type mismatch in residue
    //xlogp
    {
      xtool_ff::Nam,
      "Nam",
      14.01,
      1.750,
      0.000,
      0.000,
      "D"
    },

    //xlogp
    {
      xtool_ff::So2,
      "So2",
      32.07,
      2.000,
      0.000,
      0.000,
      "P"
    },

    //ions
    {
      xtool_ff::Li,
      "Li",
      0.00,
      1.250,
      0.000,
      1.000,
      "M"
    },

    {
      xtool_ff::Na,
      "Na",
      0.00,
      1.250,
      0.000,
      1.000,
      "M"
    },

    {
      xtool_ff::K,
      "K",
      0.00,
      1.250,
      0.000,
      1.000,
      "M"
    },

    {
      xtool_ff::Mg,
      "Mg",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Ca,
      "Ca",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Mn,
      "Mn",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Fe,
      "Fe",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Co,
      "Co",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Ni,
      "Ni",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Cu,
      "Cu",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Zn,
      "Zn",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Cd,
      "Cd",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Hg,
      "Hg",
      0.00,
      1.250,
      0.000,
      2.000,
      "M"
    },

    {
      xtool_ff::Al,
      "Al",
      0.00,
      1.250,
      0.000,
      3.000,
      "M"
    },

    {
      xtool_ff::U,
      "U",
      0.00,
      1.250,
      0.000,
      3.000,
      "M"
    },

    {
      xtool_ff::Fminus,
      "Fminus",
      19.00,
      1.500,
      0.000,
      -1.000,
      "P"
    },

    {
      xtool_ff::Clminus,
      "Clminus",
      35.45,
      1.750,
      0.000,
      -1.000,
      "P"
    },

    {
      xtool_ff::Brminus,
      "Brminus",
      79.90,
      1.900,
      0.000,
      -1.000,
      "P"
    },

    {
      xtool_ff::Iminus,
      "Iminus",
      126.90,
      2.050,
      0.000,
      -1.000,
      "P"
    }

  }};

} // namespace mudock