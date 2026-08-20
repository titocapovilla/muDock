#pragma once

#include <mudock/type_alias.hpp>

namespace mudock {

  // Layout of the raw X-Score energy terms (those calculated in X-Score's Calculate_Binding_Score)
  // used to access the output array of the kernel (terms_b) containing the respective scores
  enum x_score_term : int {
    x_term_vdw = 0, // van der Waals        (Calculate_VDW)
    x_term_hb  = 1, // hydrogen bond        (Calculate_HB)
    x_term_hp  = 2, // hydrophobic pairwise (Calculate_HP)
    x_term_rt  = 3, // rotor penalty        (Calculate_RT)
    x_term_pkd = 4, // predicted -log(Kd)   (the HPScore regression over the four terms above)
    x_term_count = 5
  };

  // Regression coefficients of X-Score's HPScore, whose terms (vdw, hb, hp, rt) are all implemented.
  //
  //   pKd = c0 + cvdw*vdw + chb*hb + chp*hp + crt*rt          (XScore's pkd1)
  //
  // XScore's other two functions (HMScore, HSScore) replace the hp term with hm/hs and are not implemented
  // muDock reports this single pKd rather than XScore's 3-function average.
  static constexpr fp_type x_hpscore_cvdw = fp_type{0.004};
  static constexpr fp_type x_hpscore_chb  = fp_type{0.054};
  static constexpr fp_type x_hpscore_chp  = fp_type{0.009};
  static constexpr fp_type x_hpscore_crt  = fp_type{-0.061};
  static constexpr fp_type x_hpscore_c0   = fp_type{3.441};

  // Combine the four raw terms into the predicted binding affinity, -log(Kd).
  [[nodiscard]] constexpr fp_type compute_x_score_pkd(const fp_type vdw,
                                                      const fp_type hb,
                                                      const fp_type hp,
                                                      const fp_type rt) {
    return x_hpscore_c0 + x_hpscore_cvdw * vdw + x_hpscore_chb * hb + x_hpscore_chp * hp +
           x_hpscore_crt * rt;
  }

} // namespace mudock
