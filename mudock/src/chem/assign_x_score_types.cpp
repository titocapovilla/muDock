#include <mudock/chem/assign_x_score_types.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/molecule/graph.hpp>


namespace mudock {

  //===------------------------------------------------------------------------------------------------------
  // Helper predicates for neighbor classification, mirroring XScore's Find_A_Group logic.
  // XScore checks neighbor SYBYL type strings (e.g. type[0]=='H', type=="F", type[0]=='N').
  // Here we check the equivalent xtool_ff enum values assigned by ob_wrapper.
  //===------------------------------------------------------------------------------------------------------

  // Returns true if the neighbor xtool_ff type represents a hydrogen atom.
  // Mirrors XScore: group.neib[i].type[0]=='H'
  static bool is_hydrogen_type(xtool_ff t) {
    return t == xtool_ff::H || t == xtool_ff::Hhb;
  }

  // Returns true if the neighbor xtool_ff type represents a heteroatom
  // (F, Cl, Br, I, or any N/O/P/S type — but NOT Si or C).
  // Mirrors XScore's Find_A_Group hetero counting (lines 1492-1504):
  //   F, Cl, Br, I → hetero++
  //   Si → continue (skip)
  //   type[0]=='N' → hetero++
  //   type[0]=='O' → hetero++
  //   type[0]=='P' → hetero++
  //   type[0]=='S' → hetero++
  static bool is_heteroatom_type(xtool_ff t) {
    switch(t) {
      // Halogens
      case xtool_ff::F:
      case xtool_ff::Cl:
      case xtool_ff::Br:
      case xtool_ff::I:
      // Nitrogen types (all N.* SYBYL types → type[0]=='N')
      case xtool_ff::N4:
      case xtool_ff::N3:
      case xtool_ff::N3h:
      case xtool_ff::N3un:
      case xtool_ff::Npl3:
      case xtool_ff::Npl3h:
      case xtool_ff::Npl3un:
      case xtool_ff::N2:
      case xtool_ff::N2h:
      case xtool_ff::N2un:
      case xtool_ff::Nar:
      case xtool_ff::Narh:
      case xtool_ff::Narun:
      case xtool_ff::N1:
      case xtool_ff::N1un:
      case xtool_ff::Nam:
      // Oxygen types (all O.* SYBYL types → type[0]=='O')
      case xtool_ff::O3:
      case xtool_ff::O3h:
      case xtool_ff::O3un:
      case xtool_ff::O2:
      case xtool_ff::O2un:
      case xtool_ff::Oco2:
      case xtool_ff::Ow:
      // Phosphorus types (P.3 → type[0]=='P')
      case xtool_ff::P3:
      // Sulfur types (all S.* SYBYL types → type[0]=='S')
      case xtool_ff::S3:
      case xtool_ff::S3h:
      case xtool_ff::S3un:
      case xtool_ff::S2:
      case xtool_ff::S2un:
      case xtool_ff::So:
      case xtool_ff::So2:
        return true;
      default:
        return false;
    }
  }

  // Returns true if the neighbor xtool_ff type represents an oxygen or nitrogen type.
  // Used for hydrogen H.hb detection:
  // XScore: if(group.neib[0].type[0]=='O') or if(group.neib[0].type[0]=='N')
  static bool is_oxygen_or_nitrogen_type(xtool_ff t) {
    switch(t) {
      case xtool_ff::O3:
      case xtool_ff::O3h:
      case xtool_ff::O3un:
      case xtool_ff::O2:
      case xtool_ff::O2un:
      case xtool_ff::Oco2:
      case xtool_ff::Ow:
      case xtool_ff::N4:
      case xtool_ff::N3:
      case xtool_ff::N3h:
      case xtool_ff::N3un:
      case xtool_ff::Npl3:
      case xtool_ff::Npl3h:
      case xtool_ff::Npl3un:
      case xtool_ff::N2:
      case xtool_ff::N2h:
      case xtool_ff::N2un:
      case xtool_ff::Nar:
      case xtool_ff::Narh:
      case xtool_ff::Narun:
      case xtool_ff::N1:
      case xtool_ff::N1un:
      case xtool_ff::Nam:
        return true;
      default:
        return false;
    }
  }

  //===------------------------------------------------------------------------------------------------------
  // Equivalent to XScore's Find_A_Group — counts neighbor environment properties.
  // Only the fields used by Get_XTOOL_Type are computed (num_h, num_nonh, num_hetero, is_bonded_to_ON).
  //===------------------------------------------------------------------------------------------------------
  struct atom_environment {
      int num_h = 0;
      int num_nonh = 0;
      int num_hetero = 0;
      // Only needed for Hydrogen atoms to determine if they are Hydrogen Bond donors (H.hb)
      bool is_bonded_to_ON = false;
  };

  //===------------------------------------------------------------------------------------------------------
  // LIGAND SPECIALIZATION
  // Mirrors XScore's Get_XTOOL_Type 1:1 by predicating on xtool_ff types (SYBYL equivalents)
  // stored in mol.atom_type(i) by the ob_wrapper, rather than on periodic table elements.
  //===------------------------------------------------------------------------------------------------------
  template<>
  void assign_x_score_types(x_score_static_layer& layer) {
      // Retrieve access to the underlying molecule's SoA (Structure of Arrays) data
      const auto& mol = layer.get_base_molecule();
      const std::size_t num_atoms = mol.num_atoms();
      // Get the SYBYL-equivalent xtool_ff types assigned by ob_wrapper
      const auto sybyl_types = mol.get_atom_type();

      // Create the topological graph on-the-fly using the bond array.
      // This provides O(1) neighbor lookups and is deallocated at function exit.
      const auto graph = make_graph(mol.get_bonds(), num_atoms);

      for (std::size_t i = 0; i < num_atoms; ++i) {
          atom_environment env;

          // ---- Find_A_Group equivalent ----
          // Iterate over the neighbors of atom 'i' and count by their xtool_ff type,
          // mirroring XScore which checks SYBYL type strings.
          auto [vi, vi_end] = boost::adjacent_vertices(i, graph);
          for (; vi != vi_end; ++vi) {
              const int neighbor_idx = *vi;
              const xtool_ff neib_type = sybyl_types[neighbor_idx];

              if (is_hydrogen_type(neib_type)) {
                  env.num_h++;
              } else {
                  env.num_nonh++;
                  if (is_heteroatom_type(neib_type)) {
                      env.num_hetero++;
                  }
              }
          }

          // For hydrogen atoms: check if bonded to O or N (for H.hb)
          if (is_hydrogen_type(sybyl_types[i])) {
              auto [vi2, vi2_end] = boost::adjacent_vertices(i, graph);
              for (; vi2 != vi2_end; ++vi2) {
                  if (is_oxygen_or_nitrogen_type(sybyl_types[*vi2])) {
                      env.is_bonded_to_ON = true;
                      break;
                  }
              }
          }

          // ---- Get_XTOOL_Type equivalent ----
          // Mirror XScore's if-chain exactly, predicating on the SYBYL type (xtool_ff)
          // of the center atom. Each block corresponds to a specific XScore type check.
          xtool_ff assigned_type = xtool_ff::Un;
          const xtool_ff center_type = sybyl_types[i];
          const bool is_aromatic = mol.is_aromatic(i); // equivalent to atom.ring==2 in XScore

          // XScore: if(!strcmp(atom.type,"H")||!strcmp(atom.type,"H.spc"))
          if (is_hydrogen_type(center_type)) {
              if (env.is_bonded_to_ON) assigned_type = xtool_ff::Hhb;
              else assigned_type = xtool_ff::H;
          }

          // XScore: if(!strcmp(atom.type,"C.3"))
          if (center_type == xtool_ff::C3) {
              if (env.num_hetero == 0)      assigned_type = xtool_ff::C3;
              else if (env.num_hetero > 0)  assigned_type = xtool_ff::C3x;
              else                          assigned_type = xtool_ff::C3un;
          }

          // XScore: if(!strcmp(atom.type,"C.2")&&(atom.ring!=2))
          if (center_type == xtool_ff::C2 && !is_aromatic) {
              if (env.num_hetero == 0)      assigned_type = xtool_ff::C2;
              else if (env.num_hetero > 0)  assigned_type = xtool_ff::C2x;
              else                          assigned_type = xtool_ff::C2un;
          }

          // XScore: if(!strcmp(atom.type,"C.ar")||(!strcmp(atom.type,"C.2")&&atom.ring==2))
          if (center_type == xtool_ff::Car || (center_type == xtool_ff::C2 && is_aromatic)) {
              if (env.num_hetero == 0)      assigned_type = xtool_ff::Car;
              else if (env.num_hetero > 0)  assigned_type = xtool_ff::Carx;
              else                          assigned_type = xtool_ff::Carun;
          }

          // XScore: if(!strcmp(atom.type,"C.1"))
          if (center_type == xtool_ff::C1) {
              if (env.num_hetero == 0)      assigned_type = xtool_ff::C1;
              else if (env.num_hetero > 0)  assigned_type = xtool_ff::C1x;
              else                          assigned_type = xtool_ff::C1un;
          }

          // XScore: if(!strcmp(atom.type,"C.cat"))
          if (center_type == xtool_ff::Ccat) {
              assigned_type = xtool_ff::Ccat;
          }

          // XScore: if(!strcmp(atom.type,"N.4")||!strcmp(atom.type,"N.3"))
          if (center_type == xtool_ff::N4 || center_type == xtool_ff::N3) {
              if (env.num_nonh <= 2)        assigned_type = xtool_ff::N4;
              else if (env.num_nonh == 3)   assigned_type = xtool_ff::N3;
              else                          assigned_type = xtool_ff::N3un;
          }

          // XScore: if((!strcmp(atom.type,"N.am")&&atom.ring!=2)||
          //            (!strcmp(atom.type,"N.pl3")&&atom.ring!=2))
          if ((center_type == xtool_ff::Nam && !is_aromatic) ||
              (center_type == xtool_ff::Npl3 && !is_aromatic)) {
              if (env.num_nonh == 1)        assigned_type = xtool_ff::Npl3h;
              else if (env.num_nonh == 2)   assigned_type = xtool_ff::Npl3h;
              else if (env.num_nonh == 3)   assigned_type = xtool_ff::Npl3;
              else                          assigned_type = xtool_ff::Npl3un;
          }

          // XScore: if(!strcmp(atom.type,"N.2")&&atom.ring!=2)
          if (center_type == xtool_ff::N2 && !is_aromatic) {
              if (env.num_nonh == 1)        assigned_type = xtool_ff::N2h;
              else if (env.num_nonh == 2)   assigned_type = xtool_ff::N2;
              else                          assigned_type = xtool_ff::N2un;
          }

          // XScore: if(!strcmp(atom.type,"N.ar")||
          //            (!strcmp(atom.type,"N.2")&&atom.ring==2)||
          //            (!strcmp(atom.type,"N.pl3")&&atom.ring==2)||
          //            (!strcmp(atom.type,"N.am")&&atom.ring==2))
          if (center_type == xtool_ff::Nar ||
              (center_type == xtool_ff::N2 && is_aromatic) ||
              (center_type == xtool_ff::Npl3 && is_aromatic) ||
              (center_type == xtool_ff::Nam && is_aromatic)) {
              if (env.num_h == 1)           assigned_type = xtool_ff::Narh;
              else if (env.num_h == 0)      assigned_type = xtool_ff::Nar;
              else                          assigned_type = xtool_ff::Narun;
          }

          // XScore: if(!strcmp(atom.type,"N.1"))
          if (center_type == xtool_ff::N1) {
              if (env.num_nonh == 1)        assigned_type = xtool_ff::N1;
              else                          assigned_type = xtool_ff::N1un;
          }

          // XScore: if(!strcmp(atom.type,"O.3"))
          if (center_type == xtool_ff::O3) {
              if (env.num_nonh == 1)        assigned_type = xtool_ff::O3h;
              else if (env.num_nonh == 2)   assigned_type = xtool_ff::O3;
              else                          assigned_type = xtool_ff::O3un;
          }

          // XScore: if(!strcmp(atom.type,"O.2"))
          if (center_type == xtool_ff::O2) {
              assigned_type = xtool_ff::O2;
          }

          // XScore: if(!strcmp(atom.type,"O.co2"))
          if (center_type == xtool_ff::Oco2) {
              assigned_type = xtool_ff::Oco2;
          }

          // XScore: if(!strcmp(atom.type,"S.3"))
          if (center_type == xtool_ff::S3) {
              if (env.num_nonh == 1)        assigned_type = xtool_ff::S3h;
              else if (env.num_nonh == 2)   assigned_type = xtool_ff::S3;
              else                          assigned_type = xtool_ff::S3un;
          }

          // XScore: if(!strcmp(atom.type,"S.2"))
          if (center_type == xtool_ff::S2) {
              assigned_type = xtool_ff::S2;
          }

          // XScore: if(!strcmp(atom.type,"S.o"))
          if (center_type == xtool_ff::So) {
              assigned_type = xtool_ff::So;
          }

          // XScore: if(!strcmp(atom.type,"S.o2"))
          if (center_type == xtool_ff::So2) {
              assigned_type = xtool_ff::So;  // Note: XScore maps S.o2 → "S.o" (same as S.o)
          }

          // XScore: if(!strcmp(atom.type,"P.3"))
          if (center_type == xtool_ff::P3) {
              assigned_type = xtool_ff::P3;
          }

          // XScore: if(!strcmp(atom.type,"F"))
          if (center_type == xtool_ff::F)  assigned_type = xtool_ff::F;

          // XScore: if(!strcmp(atom.type,"Cl"))
          if (center_type == xtool_ff::Cl) assigned_type = xtool_ff::Cl;

          // XScore: if(!strcmp(atom.type,"Br"))
          if (center_type == xtool_ff::Br) assigned_type = xtool_ff::Br;

          // XScore: if(!strcmp(atom.type,"I"))
          if (center_type == xtool_ff::I)  assigned_type = xtool_ff::I;

          // XScore: if(!strcmp(atom.type,"Si"))
          if (center_type == xtool_ff::Si) assigned_type = xtool_ff::Si;


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
