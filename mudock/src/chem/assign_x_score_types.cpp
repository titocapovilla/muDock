#include <mudock/chem/assign_x_score_types.hpp>

namespace mudock {

  // LIGAND SPECIALIZATION
  template<>
  void assign_x_score_types(x_score_static_layer& layer) {
    auto& mol = layer.get_base_molecule();
    // Implementation of chemical perception (hybridization, aromaticity, etc.)
    // Writes directly to layer.atom_x_score_xtool_type[i]
  }

  // PROTEIN SPECIALIZATION
  template<>
  void assign_x_score_types(x_score_dynamic_layer& layer) {
    auto& mol = layer.get_base_molecule();
    // Implementation of residue-based lookup (Atom Name + Residue Name)
    // Writes directly to layer.atom_x_score_xtool_type[i]
  }

}
