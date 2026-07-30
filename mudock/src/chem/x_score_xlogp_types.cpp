#include <algorithm>
#include <mudock/chem/x_score_xlogp_types.hpp>
#include <stdexcept>

//===------------------------------------------------------------------------------------------------------
// WARNING: This file has been automatically generated from chem/xlogp_types.json
//===------------------------------------------------------------------------------------------------------

namespace mudock {
  xlogp_ff parse_xlogp_type(const std::string_view symbol) {
    const auto element_it = std::find_if(std::begin(XLOGP_FF_DICTIONARY),
                                         std::end(XLOGP_FF_DICTIONARY),
                                         [&symbol](const auto& e) { return e.name == symbol; });
    if (element_it != std::end(XLOGP_FF_DICTIONARY))
      return element_it->value;
    else
      throw std::runtime_error("Missing xlogp type");
  }
  const std::array<xlogp_ff_description, 112> XLOGP_FF_DICTIONARY = {{

    {
      xlogp_ff::C_3_h3_pi_eq_0,
      "C_3_h3_pi_eq_0",
      x_score_hb::H,
      0.528
    },

    {
      xlogp_ff::C_3_h3_pi_eq_1,
      "C_3_h3_pi_eq_1",
      x_score_hb::H,
      0.267
    },

    {
      xlogp_ff::C_3_h3_x,
      "C_3_h3_x",
      x_score_hb::P,
      -0.032
    },

    {
      xlogp_ff::C_3_h2_pi_eq_0,
      "C_3_h2_pi_eq_0",
      x_score_hb::H,
      0.358
    },

    {
      xlogp_ff::C_3_h2_pi_eq_1,
      "C_3_h2_pi_eq_1",
      x_score_hb::H,
      -0.008
    },

    {
      xlogp_ff::C_3_h2_pi_eq_2,
      "C_3_h2_pi_eq_2",
      x_score_hb::P,
      -0.185
    },

    {
      xlogp_ff::C_3_h2_x_pi_eq_0,
      "C_3_h2_x_pi_eq_0",
      x_score_hb::P,
      -0.137
    },

    {
      xlogp_ff::C_3_h2_x_pi_eq_1,
      "C_3_h2_x_pi_eq_1",
      x_score_hb::P,
      -0.303
    },

    {
      xlogp_ff::C_3_h2_x_pi_eq_2,
      "C_3_h2_x_pi_eq_2",
      x_score_hb::P,
      -0.815
    },

    {
      xlogp_ff::C_3_h_pi_eq_0,
      "C_3_h_pi_eq_0",
      x_score_hb::H,
      0.127
    },

    {
      xlogp_ff::C_3_h_pi_eq_1,
      "C_3_h_pi_eq_1",
      x_score_hb::H,
      -0.243
    },

    {
      xlogp_ff::C_3_h_pi_gt_1,
      "C_3_h_pi_gt_1",
      x_score_hb::P,
      -0.499
    },

    {
      xlogp_ff::C_3_h_x_pi_eq_0,
      "C_3_h_x_pi_eq_0",
      x_score_hb::P,
      -0.205
    },

    {
      xlogp_ff::C_3_h_x_pi_eq_1,
      "C_3_h_x_pi_eq_1",
      x_score_hb::P,
      -0.305
    },

    {
      xlogp_ff::C_3_h_x_pi_gt_1,
      "C_3_h_x_pi_gt_1",
      x_score_hb::P,
      -0.709
    },

    {
      xlogp_ff::C_3_pi_eq_0,
      "C_3_pi_eq_0",
      x_score_hb::H,
      -0.006
    },

    {
      xlogp_ff::C_3_pi_eq_1,
      "C_3_pi_eq_1",
      x_score_hb::H,
      -0.570
    },

    {
      xlogp_ff::C_3_pi_gt_1,
      "C_3_pi_gt_1",
      x_score_hb::P,
      -0.317
    },

    {
      xlogp_ff::C_3_x_pi_eq_0,
      "C_3_x_pi_eq_0",
      x_score_hb::P,
      -0.316
    },

    {
      xlogp_ff::C_3_x_pi_gt_0,
      "C_3_x_pi_gt_0",
      x_score_hb::P,
      -0.723
    },

    {
      xlogp_ff::C_3_unknown,
      "C_3_unknown",
      x_score_hb::P,
      0.528
    },

    {
      xlogp_ff::C_2_h2,
      "C_2_h2",
      x_score_hb::H,
      0.420
    },

    {
      xlogp_ff::C_2_h_pi_eq_0,
      "C_2_h_pi_eq_0",
      x_score_hb::H,
      0.466
    },

    {
      xlogp_ff::C_2_h_pi_eq_1,
      "C_2_h_pi_eq_1",
      x_score_hb::H,
      0.136
    },

    {
      xlogp_ff::C_2_h_x_pi_eq_0,
      "C_2_h_x_pi_eq_0",
      x_score_hb::P,
      0.001
    },

    {
      xlogp_ff::C_2_h_x_pi_eq_1,
      "C_2_h_x_pi_eq_1",
      x_score_hb::P,
      -0.310
    },

    {
      xlogp_ff::C_2_pi_eq_0,
      "C_2_pi_eq_0",
      x_score_hb::H,
      0.050
    },

    {
      xlogp_ff::C_2_pi_gt_0,
      "C_2_pi_gt_0",
      x_score_hb::H,
      0.013
    },

    {
      xlogp_ff::C_2_x_pi_eq_0,
      "C_2_x_pi_eq_0",
      x_score_hb::P,
      -0.030
    },

    {
      xlogp_ff::C_2_x_pi_gt_0,
      "C_2_x_pi_gt_0",
      x_score_hb::P,
      -0.027
    },

    {
      xlogp_ff::C_2_x2_pi_eq_0,
      "C_2_x2_pi_eq_0",
      x_score_hb::P,
      0.005
    },

    {
      xlogp_ff::C_2_x2_pi_gt_0,
      "C_2_x2_pi_gt_0",
      x_score_hb::P,
      -0.315
    },

    {
      xlogp_ff::C_2_unknown,
      "C_2_unknown",
      x_score_hb::P,
      0.050
    },

    {
      xlogp_ff::C_ar_h,
      "C_ar_h",
      x_score_hb::H,
      0.337
    },

    {
      xlogp_ff::C_ar_h_X,
      "C_ar_h_X",
      x_score_hb::H,
      0.126
    },

    {
      xlogp_ff::C_ar,
      "C_ar",
      x_score_hb::H,
      0.296
    },

    {
      xlogp_ff::C_ar_x,
      "C_ar_x",
      x_score_hb::P,
      -0.151
    },

    {
      xlogp_ff::C_ar_X,
      "C_ar_X",
      x_score_hb::H,
      0.174
    },

    {
      xlogp_ff::C_ar_X_x,
      "C_ar_X_x",
      x_score_hb::H,
      0.366
    },

    {
      xlogp_ff::C_ar_unknown,
      "C_ar_unknown",
      x_score_hb::P,
      0.296
    },

    {
      xlogp_ff::C_1_h,
      "C_1_h",
      x_score_hb::H,
      0.209
    },

    {
      xlogp_ff::C_1,
      "C_1",
      x_score_hb::H,
      0.330
    },

    {
      xlogp_ff::C_1_eq_eq,
      "C_1_eq_eq",
      x_score_hb::H,
      2.073
    },

    {
      xlogp_ff::C_1_unknown,
      "C_1_unknown",
      x_score_hb::H,
      0.330
    },

    {
      xlogp_ff::C_cat,
      "C_cat",
      x_score_hb::P,
      -0.315
    },

    {
      xlogp_ff::N_3_h2_pi_eq_0,
      "N_3_h2_pi_eq_0",
      x_score_hb::D,
      -0.534
    },

    {
      xlogp_ff::N_3_h2_pi_eq_1,
      "N_3_h2_pi_eq_1",
      x_score_hb::D,
      -0.329
    },

    {
      xlogp_ff::N_3_h2_x,
      "N_3_h2_x",
      x_score_hb::D,
      -1.082
    },

    {
      xlogp_ff::N_3_h_pi_eq_0,
      "N_3_h_pi_eq_0",
      x_score_hb::D,
      -0.112
    },

    {
      xlogp_ff::N_3_h_pi_gt_0,
      "N_3_h_pi_gt_0",
      x_score_hb::D,
      0.166
    },

    {
      xlogp_ff::N_3_h_ring,
      "N_3_h_ring",
      x_score_hb::D,
      0.545
    },

    {
      xlogp_ff::N_3_h_x,
      "N_3_h_x",
      x_score_hb::D,
      0.324
    },

    {
      xlogp_ff::N_3_h_x_ring,
      "N_3_h_x_ring",
      x_score_hb::D,
      0.153
    },

    {
      xlogp_ff::N_3_pi_eq_0,
      "N_3_pi_eq_0",
      x_score_hb::P,
      0.159
    },

    {
      xlogp_ff::N_3_pi_gt_0,
      "N_3_pi_gt_0",
      x_score_hb::P,
      0.761
    },

    {
      xlogp_ff::N_3_ring,
      "N_3_ring",
      x_score_hb::P,
      0.881
    },

    {
      xlogp_ff::N_3_x,
      "N_3_x",
      x_score_hb::P,
      -0.239
    },

    {
      xlogp_ff::N_3_x_ring,
      "N_3_x_ring",
      x_score_hb::P,
      -0.010
    },

    {
      xlogp_ff::N_3_unknown,
      "N_3_unknown",
      x_score_hb::P,
      0.159
    },

    {
      xlogp_ff::N_am_h2,
      "N_am_h2",
      x_score_hb::D,
      -0.646
    },

    {
      xlogp_ff::N_am_h,
      "N_am_h",
      x_score_hb::D,
      -0.096
    },

    {
      xlogp_ff::N_am_h_x,
      "N_am_h_x",
      x_score_hb::D,
      -0.044
    },

    {
      xlogp_ff::N_am,
      "N_am",
      x_score_hb::P,
      0.078
    },

    {
      xlogp_ff::N_am_x,
      "N_am_x",
      x_score_hb::P,
      -0.118
    },

    {
      xlogp_ff::N_am_unknown,
      "N_am_unknown",
      x_score_hb::P,
      0.078
    },

    {
      xlogp_ff::N_2_eq_C_pi_eq_0,
      "N_2_eq_C_pi_eq_0",
      x_score_hb::A,
      0.007
    },

    {
      xlogp_ff::N_2_eq_C_pi_eq_1,
      "N_2_eq_C_pi_eq_1",
      x_score_hb::A,
      -0.275
    },

    {
      xlogp_ff::N_2_eq_C_x_pi_eq_0,
      "N_2_eq_C_x_pi_eq_0",
      x_score_hb::A,
      0.366
    },

    {
      xlogp_ff::N_2_eq_C_x_pi_eq_1,
      "N_2_eq_C_x_pi_eq_1",
      x_score_hb::A,
      0.251
    },

    {
      xlogp_ff::N_2_eq_N,
      "N_2_eq_N",
      x_score_hb::A,
      0.536
    },

    {
      xlogp_ff::N_2_eq_N_x,
      "N_2_eq_N_x",
      x_score_hb::A,
      -0.597
    },

    {
      xlogp_ff::N_2_o,
      "N_2_o",
      x_score_hb::P,
      0.427
    },

    {
      xlogp_ff::N_2_o2,
      "N_2_o2",
      x_score_hb::P,
      1.178
    },

    {
      xlogp_ff::N_2_unknown,
      "N_2_unknown",
      x_score_hb::P,
      0.007
    },

    {
      xlogp_ff::N_ar,
      "N_ar",
      x_score_hb::A,
      -0.493
    },

    {
      xlogp_ff::N_1,
      "N_1",
      x_score_hb::A,
      -0.566
    },

    {
      xlogp_ff::N_4,
      "N_4",
      x_score_hb::D,
      -0.534
    },

    {
      xlogp_ff::O_3_h_pi_eq_0,
      "O_3_h_pi_eq_0",
      x_score_hb::DA,
      -0.467
    },

    {
      xlogp_ff::O_3_h_pi_eq_1,
      "O_3_h_pi_eq_1",
      x_score_hb::DA,
      0.082
    },

    {
      xlogp_ff::O_3_h_x,
      "O_3_h_x",
      x_score_hb::DA,
      -0.522
    },

    {
      xlogp_ff::O_3_pi_eq_0,
      "O_3_pi_eq_0",
      x_score_hb::A,
      0.084
    },

    {
      xlogp_ff::O_3_pi_gt_0,
      "O_3_pi_gt_0",
      x_score_hb::A,
      0.435
    },

    {
      xlogp_ff::O_3_x,
      "O_3_x",
      x_score_hb::A,
      0.105
    },

    {
      xlogp_ff::O_3_unknown,
      "O_3_unknown",
      x_score_hb::P,
      0.084
    },

    {
      xlogp_ff::O_2,
      "O_2",
      x_score_hb::A,
      -0.399
    },

    {
      xlogp_ff::O_co2,
      "O_co2",
      x_score_hb::DA,
      -0.399
    },

    {
      xlogp_ff::S_3_h,
      "S_3_h",
      x_score_hb::H,
      0.419
    },

    {
      xlogp_ff::S_3,
      "S_3",
      x_score_hb::H,
      0.255
    },

    {
      xlogp_ff::S_3_unknown,
      "S_3_unknown",
      x_score_hb::H,
      0.255
    },

    {
      xlogp_ff::S_2,
      "S_2",
      x_score_hb::P,
      -0.148
    },

    {
      xlogp_ff::S_o,
      "S_o",
      x_score_hb::P,
      -1.375
    },

    {
      xlogp_ff::S_o2,
      "S_o2",
      x_score_hb::P,
      -0.168
    },

    {
      xlogp_ff::P_3_eq_O,
      "P_3_eq_O",
      x_score_hb::P,
      -0.447
    },

    {
      xlogp_ff::P_3_eq_S,
      "P_3_eq_S",
      x_score_hb::P,
      1.253
    },

    {
      xlogp_ff::P_3_unknown,
      "P_3_unknown",
      x_score_hb::P,
      -0.447
    },

    {
      xlogp_ff::F_pi_eq_0,
      "F_pi_eq_0",
      x_score_hb::H,
      0.375
    },

    {
      xlogp_ff::F_pi_eq_1,
      "F_pi_eq_1",
      x_score_hb::H,
      0.202
    },

    {
      xlogp_ff::F_unknown,
      "F_unknown",
      x_score_hb::H,
      0.375
    },

    {
      xlogp_ff::Cl_pi_eq_0,
      "Cl_pi_eq_0",
      x_score_hb::H,
      0.512
    },

    {
      xlogp_ff::Cl_pi_eq_1,
      "Cl_pi_eq_1",
      x_score_hb::H,
      0.663
    },

    {
      xlogp_ff::Cl_unknown,
      "Cl_unknown",
      x_score_hb::H,
      0.512
    },

    {
      xlogp_ff::Br_pi_eq_0,
      "Br_pi_eq_0",
      x_score_hb::H,
      0.850
    },

    {
      xlogp_ff::Br_pi_eq_1,
      "Br_pi_eq_1",
      x_score_hb::H,
      0.839
    },

    {
      xlogp_ff::Br_unknown,
      "Br_unknown",
      x_score_hb::H,
      0.850
    },

    {
      xlogp_ff::I_pi_eq_0,
      "I_pi_eq_0",
      x_score_hb::H,
      1.050
    },

    {
      xlogp_ff::I_pi_eq_1,
      "I_pi_eq_1",
      x_score_hb::H,
      1.109
    },

    {
      xlogp_ff::I_unknown,
      "I_unknown",
      x_score_hb::H,
      1.050
    },

    {
      xlogp_ff::H,
      "H",
      x_score_hb::N,
      0.000
    },

    {
      xlogp_ff::H_hb,
      "H_hb",
      x_score_hb::DH,
      0.000
    },

    {
      xlogp_ff::Si,
      "Si",
      x_score_hb::N,
      0.000
    },

    {
      xlogp_ff::Un,
      "Un",
      x_score_hb::N,
      0.000
    },

    {
      xlogp_ff::Du,
      "Du",
      x_score_hb::N,
      0.000
    }

  }};

} // namespace mudock