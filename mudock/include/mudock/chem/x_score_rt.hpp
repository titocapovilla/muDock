#pragma once

#include <mudock/chem/x_score_ligand.hpp>
#include <mudock/type_alias.hpp>

namespace mudock {

  // Rotor (RT) term of the X-Score scoring function, ported from Ligand::Calculate_RT (score.cpp).
  //
  // The term is a property of the ligand (it does not depend on the protein), so it is
  // computed once per ligand on the host (X-Score's prepare()). 
  // It counts the "frozen" single rotatable bonds of the ligand, after discarding terminal
  // rotors, sp2-sp2 rotors, bonds to terminal heavy atoms and ring bonds, then distributes a per-atom
  // penalty (0.5 / 1.0 / 0.5 for atoms carrying 1 / 2 / >=3 rotors).

  [[nodiscard]] fp_type compute_x_score_rt(const x_score_ligand& xs_lig);

} // namespace mudock
