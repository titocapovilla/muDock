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

    for (int i = 0; i < mol.num_atoms(); ++i) {
        residue res_type = mol.residue_types(i);
        std::string_view atom_name = mol.atom_name(i);

        // Get the blueprint for this residue
        const auto& res_desc = get_description(res_type);

        // Match the atom name to the dictionary entry
        bool found = false;
        for (const auto& atom_tmpl : res_desc.atoms) {
            if (atom_tmpl.name == atom_name) {
                // Assign the X-Score types found in the dictionary
                layer.x_score_xtool_type(i) = atom_tmpl.x_tool_atom_type;
                //logp type can also be assigned here if needed
                found = true;
                break;
            }
        }

        if (!found) {
            // Handle cases where PDB name doesn't match
            layer.x_score_xtool_type(i) = xtool_ff::Un;
        }
    }
  }

}
