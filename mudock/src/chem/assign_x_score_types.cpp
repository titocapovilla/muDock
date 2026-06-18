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

  // the functions tries to mirror x-score but only uses element instead of types
  template<>
  void assign_x_score_types(x_score_static_layer& layer) {
      // Retrieve access to the underlying molecule's SoA (Structure of Arrays) data
      const auto& mol = layer.get_base_molecule();
      const std::size_t num_atoms = mol.num_atoms();
      const auto elements = mol.get_elements();

      // The span to write the results
      auto xtool_types = layer.get_x_score_xtool_type();

      // Create the topological graph on-the-fly using the bond array.
      // This provides O(1) neighbor lookups and is deallocated at function exit.
      const auto graph = make_graph(mol.get_bonds(), num_atoms);

      for (std::size_t i = 0; i < num_atoms; ++i) {
          atom_environment env;

          // Find_A_Group logic in x-score
          // Iterate over the neighbors of atom 'i' by querying the graph
          auto [vi, vi_end] = boost::adjacent_vertices(i, graph);
          for (; vi != vi_end; ++vi) {
              int neighbor_idx = *vi;
              auto el = elements[neighbor_idx];

              if (el == element::H) {
                  env.num_h++;
              } else {
                  env.num_nonh++;

                  // Track if neighbor is a heteroatom
                  if (el == element::F || el == element::Cl || el == element::Br ||
                      el == element::I || el == element::N || el == element::O ||
                      el == element::P || el == element::S) {
                    env.num_hetero++;
                  }

                  // If the current atom 'i' is Hydrogen, check if it's bonded to Oxygen or Nitrogen
                  if (elements[i] == element::H && (el == element::O || el == element::N)) {
                    env.is_bonded_to_ON = true;
                  }
              }
          }


      // Get_XTOOL_Type
      xtool_ff assigned_type = xtool_ff::Un;
      auto base_el = elements[i];
      int total_bonds = env.num_nonh + env.num_h; // Used to deduce hybridization
      // Hydrogen typing
      if (base_el == element::H) {
          if (env.is_bonded_to_ON) assigned_type = xtool_ff::H_hb;
          else assigned_type = xtool_ff::H;
      }
      // Carbon typing
      else if (base_el == element::C) {
          if (mol.is_aromatic(i)) { // Equivalent to C.ar or (C.2 and atom.ring==2)
              if (env.num_hetero == 0) assigned_type = xtool_ff::C_ar;
              else if (env.num_hetero > 0) assigned_type = xtool_ff::C_ar_x;
              else assigned_type = xtool_ff::C_ar_un;
          } else {
              if (total_bonds == 4) { // Equivalent to C.3 (sp3)
                  if (env.num_hetero == 0) assigned_type = xtool_ff::C_3;
                  else if (env.num_hetero > 0) assigned_type = xtool_ff::C_3_x;
                  else assigned_type = xtool_ff::C_3_un;
              } else if (total_bonds == 3) { // Equivalent to C.2 (sp2)
                  if (env.num_hetero == 0) assigned_type = xtool_ff::C_2;
                  else if (env.num_hetero > 0) assigned_type = xtool_ff::C_2_x;
                  else assigned_type = xtool_ff::C_2_un;
              } else if (total_bonds <= 2) { // Equivalent to C.1 (sp)
                  if (env.num_hetero == 0) assigned_type = xtool_ff::C_1;
                  else if (env.num_hetero > 0) assigned_type = xtool_ff::C_1_x;
                  else assigned_type = xtool_ff::C_1_un;
              }
          }
      }
      // Nitrogen typing
      else if (base_el == element::N) {
          if (mol.is_aromatic(i)) { // Equivalent to N.ar
              if (env.num_h == 1) assigned_type = xtool_ff::N_ar_h;
              else if (env.num_h == 0) assigned_type = xtool_ff::N_ar;
              else assigned_type = xtool_ff::N_ar_un;
          } else {
              if (total_bonds >= 3) { // Equivalent to N.4, N.3, or N.pl3
                if (env.num_nonh <= 2) assigned_type = xtool_ff::N_4;
                else if (env.num_nonh == 3) assigned_type = xtool_ff::N_3;
                else assigned_type = xtool_ff::N_3_un;
              } else if (total_bonds == 2) { // Equivalent to N.2
                if (env.num_nonh == 1) assigned_type = xtool_ff::N_2_h;
                else if (env.num_nonh == 2) assigned_type = xtool_ff::N_2;
                else assigned_type = xtool_ff::N_2_un;
              } else if (total_bonds == 1) { // Equivalent to N.1
                if (env.num_nonh == 1) assigned_type = xtool_ff::N_1;
                else assigned_type = xtool_ff::N_1_un;
              }
          }
      }
      // Oxygen typing
      else if (base_el == element::O) {
          if (total_bonds >= 2) { // Equivalent to O.3
              if (env.num_nonh == 1) assigned_type = xtool_ff::O_3_h;
              else if (env.num_nonh == 2) assigned_type = xtool_ff::O_3;
              else assigned_type = xtool_ff::O_3_un;
          } else {
              // Equivalent to O.2 (Note: for exact O.co2 carboxylate mapping you'd check if bonded to C=O)
              assigned_type = xtool_ff::O_2;
          }
      }
      // Sulfur typing
      else if (base_el == element::S) {
          if (total_bonds >= 2) { // Equivalent to S.3
              if (env.num_nonh == 1) assigned_type = xtool_ff::S_3_h;
              else if (env.num_nonh == 2) assigned_type = xtool_ff::S_3;
              else assigned_type = xtool_ff::S_3_un;
          } else { // Equivalent to S.2, S.o, S.o2
              assigned_type = xtool_ff::S_2;
          }
      }
      // Phosphorus typing
      else if (base_el == element::P) {
          assigned_type = xtool_ff::P_3;
      }
      // Halogens and others
      else if (base_el == element::F) assigned_type = xtool_ff::F;
      else if (base_el == element::Cl) assigned_type = xtool_ff::Cl;
      else if (base_el == element::Br) assigned_type = xtool_ff::Br;
      else if (base_el == element::I) assigned_type = xtool_ff::I;
      else if (base_el == element::Si) assigned_type = xtool_ff::Si;


      // DATA SAVING
      layer.x_score_xtool_type(i) = assigned_type;
    }
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
