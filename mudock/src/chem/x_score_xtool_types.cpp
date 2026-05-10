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
      xtool_ff::C_3,
      "C_3",
      12.01,
      2.100,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::C_3_x,
      "C_3_x",
      12.01,
      2.100,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_3_un,
      "C_3_un",
      12.01,
      2.100,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_2,
      "C_2",
      12.01,
      1.900,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::C_2_x,
      "C_2_x",
      12.01,
      1.900,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_2_un,
      "C_2_un",
      12.01,
      1.900,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_ar,
      "C_ar",
      12.01,
      2.000,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::C_ar_x,
      "C_ar_x",
      12.01,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_ar_un,
      "C_ar_un",
      12.01,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_1,
      "C_1",
      12.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_1_x,
      "C_1_x",
      12.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_1_un,
      "C_1_un",
      12.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::C_cat,
      "C_cat",
      12.01,
      1.900,
      0.000,
      1.000,
      "P"
    },

    {
      xtool_ff::N_3_h,
      "N_3_h",
      14.01,
      1.800,
      0.000,
      0.000,
      "D"
    },

    {
      xtool_ff::N_3,
      "N_3",
      14.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N_3_un,
      "N_3_un",
      14.01,
      1.800,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N_pl3_h,
      "N_pl3_h",
      14.01,
      1.750,
      0.000,
      0.000,
      "D"
    },

    {
      xtool_ff::N_pl3,
      "N_pl3",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N_pl3_un,
      "N_pl3_un",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N_2_h,
      "N_2_h",
      14.01,
      1.750,
      0.000,
      0.000,
      "DA"
    },

    {
      xtool_ff::N_2,
      "N_2",
      14.01,
      1.750,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::N_2_un,
      "N_2_un",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N_ar_h,
      "N_ar_h",
      14.01,
      1.750,
      0.000,
      0.000,
      "D"
    },

    {
      xtool_ff::N_ar,
      "N_ar",
      14.01,
      1.750,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::N_ar_un,
      "N_ar_un",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N_1,
      "N_1",
      14.01,
      1.750,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::N_1_un,
      "N_1_un",
      14.01,
      1.750,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::N_4,
      "N_4",
      14.01,
      1.800,
      0.000,
      1.000,
      "D"
    },

    {
      xtool_ff::O_3_h,
      "O_3_h",
      16.00,
      1.650,
      0.000,
      0.000,
      "DA"
    },

    {
      xtool_ff::O_3,
      "O_3",
      16.00,
      1.650,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::O_3_un,
      "O_3_un",
      16.00,
      1.650,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::O_2,
      "O_2",
      16.00,
      1.550,
      0.000,
      0.000,
      "A"
    },

    {
      xtool_ff::O_2_un,
      "O_2_un",
      16.00,
      1.550,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::O_co2,
      "O_co2",
      16.00,
      1.550,
      0.000,
      -0.500,
      "DA"
    },

    {
      xtool_ff::S_3_h,
      "S_3_h",
      32.07,
      2.100,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::S_3,
      "S_3",
      32.07,
      2.100,
      0.000,
      0.000,
      "H"
    },

    {
      xtool_ff::S_3_un,
      "S_3_un",
      32.07,
      2.100,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::S_2,
      "S_2",
      32.07,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::S_2_un,
      "S_2_un",
      32.07,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::S_o,
      "S_o",
      32.07,
      2.000,
      0.000,
      0.000,
      "P"
    },

    {
      xtool_ff::P_3,
      "P_3",
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
      xtool_ff::H_hb,
      "H_hb",
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
      xtool_ff::O_w,
      "O_w",
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
      xtool_ff::N_am,
      "N_am",
      14.01,
      1.750,
      0.000,
      0.000,
      "D"
    },

    //xlogp
    {
      xtool_ff::S_o2,
      "S_o2",
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