#pragma once

#include <mudock/type_alias.hpp>

namespace mudock {

  // Layout of the raw X-Score energy terms (those calculated in X-Score's Calculate_Binding_Score)
  //
  enum x_score_term : int {
    x_term_vdw = 0, // van der Waals        (Calculate_VDW)
    x_term_hp  = 1, // hydrophobic pairwise (Calculate_HP)
    x_term_rt  = 2, // rotor penalty        (Calculate_RT)
    x_term_count = 3
  };

} // namespace mudock