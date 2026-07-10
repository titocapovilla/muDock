#pragma once

#include <mudock/type_alias.hpp>

namespace mudock {

  // Layout of the raw X-Score energy terms (those calculated in X-Score's Calculate_Binding_Score)
  //
  enum x_score_term : int {
    x_term_vdw = 0, // van der Waals        (Calculate_VDW)
    x_term_hb  = 1, // hydrogen bond        (Calculate_HB)
    x_term_hp  = 2, // hydrophobic pairwise (Calculate_HP)
    x_term_rt  = 3, // rotor penalty        (Calculate_RT)
    x_term_count = 4
  };

} // namespace mudock