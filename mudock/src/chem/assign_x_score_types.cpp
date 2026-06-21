#include <mudock/chem/assign_x_score_types.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/molecule/graph.hpp>


namespace mudock {

  // 1. Minimal support structure (Equivalent to Find_A_Group, optimized for XTOOL)
  struct atom_environment {
      int num_h = 0;
      int num_nonh = 0;
      int num_hetero = 0;
      // Only needed for Hydrogen atoms to determine if they are Hydrogen Bond donors (H.hb)
      bool is_bonded_to_ON = false;
  };
  // Todo: the logic for db_type, num_db, num_tb, num_pi are not implemented
  // same for group.amide logic

  // LIGAND SPECIALIZATION
  // the functions tries to mirror x-score but only uses element instead of types
  template<>
  void assign_x_score_types(x_score_static_layer& layer){

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
