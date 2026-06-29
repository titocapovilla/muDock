#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mudock/chem/assign_x_score_types.hpp>
#include <mudock/chem/sybyl_atom_types.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/molecule/graph.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace mudock {

  //===------------------------------------------------------------------------------------------------------
  // Helper predicates for neighbor classification, mirroring XScore's Find_A_Group logic.
  // XScore checks neighbor SYBYL type strings (e.g. type[0]=='H', type=="F", type[0]=='N').
  // Here we check the equivalent xtool_ff enum values assigned by ob_wrapper.
  //===------------------------------------------------------------------------------------------------------

  // Returns true if the neighbor xtool_ff type represents a hydrogen atom.
  // Mirrors XScore: group.neib[i].type[0]=='H'
  static bool is_hydrogen_type(xtool_ff t) { return t == xtool_ff::H || t == xtool_ff::Hhb; }

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
    switch (t) {
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
      case xtool_ff::So2: return true;
      default: return false;
    }
  }

  // Returns true if the neighbor xtool_ff type represents an oxygen or nitrogen type.
  // Used for hydrogen H.hb detection:
  // XScore: if(group.neib[0].type[0]=='O') or if(group.neib[0].type[0]=='N')
  static bool is_oxygen_or_nitrogen_type(xtool_ff t) {
    switch (t) {
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
      case xtool_ff::Nam: return true;
      default: return false;
    }
  }

  //===------------------------------------------------------------------------------------------------------
  // Equivalent to XScore's Find_A_Group — counts neighbor environment properties.
  // Only the fields used by Get_XTOOL_Type are computed (num_h, num_nonh, num_hetero, is_bonded_to_ON).
  //===------------------------------------------------------------------------------------------------------
  struct atom_environment {
    int num_h      = 0;
    int num_nonh   = 0;
    int num_hetero = 0;
    // Only needed for Hydrogen atoms to determine if they are Hydrogen Bond donors (H.hb)
    bool is_bonded_to_ON = false;
  };

  //===------------------------------------------------------------------------------------------------------
  // Derives the preliminary X-Tool atom typing from the authoritative SYBYL types parsed by the native
  // ADTMOL2 reader (molecule.sybyl_type), instead of re-reading the original .mol2 file via SOURCE_PATH.
  // The native reader preserves the full SYBYL token (no OpenBabel trimming), so this works uniformly for
  // batched ligands and removes the dependency on the source file. The conversion mirrors the previous
  // logic: take the canonical SYBYL token (e.g. "C.ar"), drop the dot and feed it to parse_xtool_type
  // ("Car" -> xtool_ff::Car).
  //===------------------------------------------------------------------------------------------------------
  static void assign_xtool_types_from_sybyl(x_score_static_layer& layer) {
    auto& mol           = layer.get_base_molecule();
    const int num_atoms = static_cast<int>(mol.num_atoms());

    for (int index = 0; index < num_atoms; ++index) {
      const auto sybyl = mol.sybyl_type(index);
      if (sybyl == sybyl_atom_type::UNKNOWN)
        continue; // no authoritative SYBYL token to override the default perception with

      const auto token = to_string(sybyl); // canonical SYBYL token, e.g. "C.ar"
      std::string sybyl_str;
      for (const char c: token) {
        if (c != '.')
          sybyl_str += c;
      }

      if (!sybyl_str.empty())
        mol.atom_type(index) = parse_xtool_type(sybyl_str);
    }
  }

  //===------------------------------------------------------------------------------------------------------
  // XScore-compatible aromatic ring detection.
  //
  // OpenBabel's IsAromatic() does NOT match XScore's Detect_Aromatic_Rings().
  // XScore has its own ring perception algorithm:
  //   1. Find which atoms/bonds are in rings (branch elimination)
  //   2. For each ring bond, find the smallest ring it belongs to (Look_For_A_Ring)
  //   3. Check 6-membered rings: all atoms must be C.ar/N.ar/C.2/N.2, and no two
  //      consecutive single/amide bonds allowed (Aromatic_Ring_Check_6)
  //   4. Check 5-membered rings: Hückel rule (6 pi electrons), with N.pl3/N.am/O.3/S.3
  //      contributing 2 pi electrons each (Aromatic_Ring_Check_5)
  //   5. Mark all atoms in aromatic rings as ring=2
  //
  // This implementation ports that algorithm to mudock's graph infrastructure.
  //===------------------------------------------------------------------------------------------------------

  // Check if an atom's SYBYL type qualifies as a pi contributor in a 6-membered aromatic ring.
  // XScore Aromatic_Ring_Check_6: C.ar, N.ar, C.2, N.2 each contribute 1 pi electron.
  static bool is_6ring_pi_atom(xtool_ff t) {
    return t == xtool_ff::Car || t == xtool_ff::Nar || t == xtool_ff::C2 || t == xtool_ff::N2;
  }

  // Get the pi electron count for an atom in a 5-membered ring.
  // Returns 0 if the atom type cannot participate in a 5-membered aromatic ring.
  // XScore Aromatic_Ring_Check_5:
  //   C.ar, N.ar, C.2, N.2 → 1 pi electron
  //   N.pl3, N.am, O.3, S.3 → 2 pi electrons
  static int get_5ring_pi_count(xtool_ff t) {
    switch (t) {
      case xtool_ff::Car:
      case xtool_ff::Nar:
      case xtool_ff::C2:
      case xtool_ff::N2: return 1;
      case xtool_ff::Npl3:
      case xtool_ff::Nam:
      case xtool_ff::O3:
      case xtool_ff::O3h:
      case xtool_ff::S3:
      case xtool_ff::S3h: return 2;
      default: return 0;
    }
  }

  // Check if a bond type is "single" for the purpose of aromatic ring detection.
  // XScore checks: !strcmp(bond.type,"1") || !strcmp(bond.type,"am")
  static bool is_single_or_amide_bond(bond_type bt) {
    return bt == bond_type::SINGLE || bt == bond_type::AMIDE;
  }

  // Compute per-atom XScore-compatible aromaticity flag (equivalent to atom.ring==2).
  // This replaces the use of OpenBabel's is_aromatic flag.
  static std::vector<bool> detect_xscore_aromaticity(const molecule_graph_type& graph,
                                                     const std::span<const xtool_ff> sybyl_types,
                                                     const std::span<const bond> bonds,
                                                     const std::size_t num_atoms) {
    std::vector<bool> xscore_aromatic(num_atoms, false);

    // --- Step 1: Detect which atoms are in rings (XScore's branch elimination) ---
    // atom_ring: -1 = ambiguous, 0 = not in ring, 1 = in a ring
    std::vector<int> atom_ring(num_atoms, -1);

    // Terminal atoms (degree <= 1) cannot be in rings
    for (std::size_t i = 0; i < num_atoms; ++i) {
      if (boost::degree(i, graph) <= 1) {
        atom_ring[i] = 0;
      }
    }

    // Collapse structure: repeatedly remove atoms that have <= 1 ring-eligible neighbor
    bool changed = true;
    while (changed) {
      changed = false;
      for (std::size_t i = 0; i < num_atoms; ++i) {
        if (atom_ring[i] != -1)
          continue;

        int ring_neighbor_count = 0;
        auto [vi, vi_end]       = boost::adjacent_vertices(i, graph);
        for (; vi != vi_end; ++vi) {
          if (atom_ring[*vi] != 0) {
            ring_neighbor_count++;
          }
        }
        if (ring_neighbor_count <= 1) {
          atom_ring[i] = 0;
          changed      = true;
        }
      }
    }

    // Mark remaining ambiguous atoms as in-ring
    for (std::size_t i = 0; i < num_atoms; ++i) {
      if (atom_ring[i] == -1)
        atom_ring[i] = 1;
    }

    // --- Step 2: Find rings using DFS and check aromaticity ---
    // For each bond whose both endpoints are in-ring atoms, try to find
    // a 6-membered or 5-membered ring containing it.

    // Helper: find a ring of exactly target_size containing the bond (src, dst).
    // Uses iterative DFS from dst, avoiding src until we loop back to src.
    // Returns the atom path (atom indices) if found, empty otherwise.
    auto find_ring_of_size = [&](int src, int dst, int target_size) -> std::vector<int> {
      // DFS state: (current_atom, path_so_far)
      struct dfs_state {
        int atom;
        std::vector<int> path;
      };

      std::vector<dfs_state> stack;
      stack.push_back({dst, {src, dst}});

      while (!stack.empty()) {
        auto [current, path] = std::move(stack.back());
        stack.pop_back();

        if (static_cast<int>(path.size()) > target_size)
          continue;

        auto [ni, ni_end] = boost::adjacent_vertices(current, graph);
        for (; ni != ni_end; ++ni) {
          int next = static_cast<int>(*ni);

          // If we found our way back to src with exactly target_size atoms
          if (next == src && static_cast<int>(path.size()) == target_size) {
            return path;
          }

          // Skip non-ring atoms
          if (atom_ring[next] != 1)
            continue;

          // Skip if already in path (avoid revisiting)
          if (std::find(path.begin(), path.end(), next) != path.end())
            continue;

          // Don't go back to src prematurely
          if (next == src)
            continue;

          // Only continue if we haven't exceeded target size
          if (static_cast<int>(path.size()) < target_size) {
            auto new_path = path;
            new_path.push_back(next);
            stack.push_back({next, std::move(new_path)});
          }
        }
      }
      return {};
    };

    // Helper: given a ring atom path, find the bond types along the ring edges
    auto get_ring_bond_types = [&](const std::vector<int>& ring_atoms) -> std::vector<bond_type> {
      std::vector<bond_type> ring_bonds;
      int n = static_cast<int>(ring_atoms.size());
      for (int j = 0; j < n; ++j) {
        int a1 = ring_atoms[j];
        int a2 = ring_atoms[(j + 1) % n];
        // Find the bond between a1 and a2 using graph edges
        auto [ei, ei_end] = boost::out_edges(a1, graph);
        for (; ei != ei_end; ++ei) {
          int target = static_cast<int>(boost::target(*ei, graph));
          if (target == a2) {
            ring_bonds.push_back(bonds[graph[*ei].bond_index].type);
            break;
          }
        }
      }
      return ring_bonds;
    };

    // Track which bond pairs have already been checked so we don't duplicate work
    // (XScore iterates bonds, we iterate edges)
    auto make_bond_key = [](int a, int b) -> long long {
      if (a > b)
        std::swap(a, b);
      return static_cast<long long>(a) * 1000000 + b;
    };

    // --- Check 6-membered rings first (XScore does 6 before 5) ---
    {
      std::vector<long long> checked_bonds;
      auto ei_pair = boost::edges(graph);
      for (auto ei = ei_pair.first; ei != ei_pair.second; ++ei) {
        int a1 = static_cast<int>(boost::source(*ei, graph));
        int a2 = static_cast<int>(boost::target(*ei, graph));

        // Skip bonds not in a ring
        if (atom_ring[a1] != 1 || atom_ring[a2] != 1)
          continue;

        // Skip already-aromatic bonds (already found in a ring)
        long long bk = make_bond_key(a1, a2);
        if (std::find(checked_bonds.begin(), checked_bonds.end(), bk) != checked_bonds.end())
          continue;
        checked_bonds.push_back(bk);

        auto ring_atoms = find_ring_of_size(a1, a2, 6);
        if (ring_atoms.empty())
          continue;

        // Aromatic_Ring_Check_6: all 6 atoms must be C.ar/N.ar/C.2/N.2
        bool all_pi  = true;
        int pi_count = 0;
        for (int atom_idx: ring_atoms) {
          if (is_6ring_pi_atom(sybyl_types[atom_idx])) {
            pi_count++;
          } else {
            all_pi = false;
            break;
          }
        }
        if (!all_pi || pi_count != 6)
          continue;

        // Check no two consecutive single/amide bonds
        auto ring_bond_types = get_ring_bond_types(ring_atoms);
        if (static_cast<int>(ring_bond_types.size()) != 6)
          continue;

        bool has_consecutive_single = false;
        for (int j = 0; j < 5; ++j) {
          if (is_single_or_amide_bond(ring_bond_types[j]) &&
              is_single_or_amide_bond(ring_bond_types[j + 1])) {
            has_consecutive_single = true;
            break;
          }
        }
        // Also check wrap-around (bond[5] and bond[0])
        if (!has_consecutive_single) {
          if (is_single_or_amide_bond(ring_bond_types[5]) && is_single_or_amide_bond(ring_bond_types[0])) {
            has_consecutive_single = true;
          }
        }
        if (has_consecutive_single)
          continue;

        // Mark all atoms in this ring as aromatic
        for (int atom_idx: ring_atoms) { xscore_aromatic[atom_idx] = true; }
      }
    }

    // --- Then check 5-membered rings ---
    {
      std::vector<long long> checked_bonds;
      auto ei_pair = boost::edges(graph);
      for (auto ei = ei_pair.first; ei != ei_pair.second; ++ei) {
        int a1 = static_cast<int>(boost::source(*ei, graph));
        int a2 = static_cast<int>(boost::target(*ei, graph));

        // Skip bonds not in a ring, and skip bonds already marked aromatic
        // (XScore: bond[i].ring!=1 means already aromatic or not in ring)
        if (atom_ring[a1] != 1 || atom_ring[a2] != 1)
          continue;
        if (xscore_aromatic[a1] && xscore_aromatic[a2])
          continue;

        long long bk = make_bond_key(a1, a2);
        if (std::find(checked_bonds.begin(), checked_bonds.end(), bk) != checked_bonds.end())
          continue;
        checked_bonds.push_back(bk);

        auto ring_atoms = find_ring_of_size(a1, a2, 5);
        if (ring_atoms.empty())
          continue;

        // Aromatic_Ring_Check_5: check pi electrons
        bool valid   = true;
        int total_pi = 0;
        std::vector<int> pi_per_atom(5);
        for (int j = 0; j < 5; ++j) {
          int pi = get_5ring_pi_count(sybyl_types[ring_atoms[j]]);
          if (pi == 0) {
            valid = false;
            break;
          }
          pi_per_atom[j] = pi;
          total_pi += pi;
        }
        if (!valid || total_pi != 6)
          continue; // Hückel's rule: 4n+2 = 6

        // Check no two consecutive single/amide bonds unless the middle atom
        // contributes 2 pi electrons (the "special atom" exception)
        auto ring_bond_types = get_ring_bond_types(ring_atoms);
        if (static_cast<int>(ring_bond_types.size()) != 5)
          continue;

        bool has_bad_consecutive = false;
        for (int j = 0; j < 4; ++j) {
          if (is_single_or_amide_bond(ring_bond_types[j]) &&
              is_single_or_amide_bond(ring_bond_types[j + 1])) {
            // XScore: if(pi_path[i+1]==2) continue; — the atom between the two
            // single bonds contributes 2 pi electrons, which is okay
            if (pi_per_atom[j + 1] == 2)
              continue;
            has_bad_consecutive = true;
            break;
          }
        }
        // Wrap-around check: bond[4] and bond[0], middle atom is ring_atoms[0]
        if (!has_bad_consecutive) {
          if (is_single_or_amide_bond(ring_bond_types[4]) && is_single_or_amide_bond(ring_bond_types[0])) {
            if (pi_per_atom[0] != 2) {
              has_bad_consecutive = true;
            }
          }
        }
        if (has_bad_consecutive)
          continue;

        // Mark all atoms in this ring as aromatic
        for (int atom_idx: ring_atoms) { xscore_aromatic[atom_idx] = true; }
      }
    }

    return xscore_aromatic;
  }

  //===------------------------------------------------------------------------------------------------------
  // LIGAND SPECIALIZATION
  // Mirrors XScore's Get_XTOOL_Type 1:1 by predicating on xtool_ff types (SYBYL equivalents)
  // stored in mol.atom_type(i) by the ob_wrapper, rather than on periodic table elements.
  //===------------------------------------------------------------------------------------------------------
  template<>
  void assign_x_score_types(x_score_static_layer& layer) {
    assign_xtool_types_from_sybyl(layer);

    // Retrieve access to the underlying molecule's SoA (Structure of Arrays) data
    const auto& mol             = layer.get_base_molecule();
    const std::size_t num_atoms = mol.num_atoms();
    // Get the SYBYL-equivalent xtool_ff types assigned by ob_wrapper
    const auto sybyl_types = mol.get_atom_type();

    // Create the topological graph on-the-fly using the bond array.
    // This provides O(1) neighbor lookups and is deallocated at function exit.
    const auto graph = make_graph(mol.get_bonds(), num_atoms);

    // Compute XScore-compatible aromaticity flags.
    // This replaces OpenBabel's is_aromatic, which does NOT match XScore's
    // Detect_Aromatic_Rings algorithm. XScore considers C.2, N.2, N.pl3, N.am
    // atoms in suitable rings as aromatic (ring=2), while OpenBabel may not.
    const auto xscore_aromatic = detect_xscore_aromaticity(graph, sybyl_types, mol.get_bonds(), num_atoms);

    for (std::size_t i = 0; i < num_atoms; ++i) {
      atom_environment env;

      // ---- Find_A_Group equivalent ----
      // Iterate over the neighbors of atom 'i' and count by their xtool_ff type,
      // mirroring XScore which checks SYBYL type strings.
      auto [vi, vi_end] = boost::adjacent_vertices(i, graph);
      for (; vi != vi_end; ++vi) {
        const int neighbor_idx   = *vi;
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
      xtool_ff assigned_type     = xtool_ff::Un;
      const xtool_ff center_type = sybyl_types[i];
      // Use XScore-compatible aromaticity instead of OpenBabel's is_aromatic
      const bool is_aromatic = xscore_aromatic[i];

      // XScore: if(!strcmp(atom.type,"H")||!strcmp(atom.type,"H.spc"))
      if (is_hydrogen_type(center_type)) {
        if (env.is_bonded_to_ON)
          assigned_type = xtool_ff::Hhb;
        else
          assigned_type = xtool_ff::H;
      }

      // XScore: if(!strcmp(atom.type,"C.3"))
      if (center_type == xtool_ff::C3) {
        if (env.num_hetero == 0)
          assigned_type = xtool_ff::C3;
        else if (env.num_hetero > 0)
          assigned_type = xtool_ff::C3x;
        else
          assigned_type = xtool_ff::C3un;
      }

      // XScore: if(!strcmp(atom.type,"C.2")&&(atom.ring!=2))
      if (center_type == xtool_ff::C2 && !is_aromatic) {
        if (env.num_hetero == 0)
          assigned_type = xtool_ff::C2;
        else if (env.num_hetero > 0)
          assigned_type = xtool_ff::C2x;
        else
          assigned_type = xtool_ff::C2un;
      }

      // XScore: if(!strcmp(atom.type,"C.ar")||(!strcmp(atom.type,"C.2")&&atom.ring==2))
      if (center_type == xtool_ff::Car || (center_type == xtool_ff::C2 && is_aromatic)) {
        if (env.num_hetero == 0)
          assigned_type = xtool_ff::Car;
        else if (env.num_hetero > 0)
          assigned_type = xtool_ff::Carx;
        else
          assigned_type = xtool_ff::Carun;
      }

      // XScore: if(!strcmp(atom.type,"C.1"))
      if (center_type == xtool_ff::C1) {
        if (env.num_hetero == 0)
          assigned_type = xtool_ff::C1;
        else if (env.num_hetero > 0)
          assigned_type = xtool_ff::C1x;
        else
          assigned_type = xtool_ff::C1un;
      }

      // XScore: if(!strcmp(atom.type,"C.cat"))
      if (center_type == xtool_ff::Ccat) {
        assigned_type = xtool_ff::Ccat;
      }

      // XScore: if(!strcmp(atom.type,"N.4")||!strcmp(atom.type,"N.3"))
      if (center_type == xtool_ff::N4 || center_type == xtool_ff::N3) {
        if (env.num_nonh <= 2)
          assigned_type = xtool_ff::N4;
        else if (env.num_nonh == 3)
          assigned_type = xtool_ff::N3;
        else
          assigned_type = xtool_ff::N3un;
      }

      // XScore: if((!strcmp(atom.type,"N.am")&&atom.ring!=2)||
      //            (!strcmp(atom.type,"N.pl3")&&atom.ring!=2))
      if ((center_type == xtool_ff::Nam && !is_aromatic) || (center_type == xtool_ff::Npl3 && !is_aromatic)) {
        if (env.num_nonh == 1)
          assigned_type = xtool_ff::Npl3h;
        else if (env.num_nonh == 2)
          assigned_type = xtool_ff::Npl3h;
        else if (env.num_nonh == 3)
          assigned_type = xtool_ff::Npl3;
        else
          assigned_type = xtool_ff::Npl3un;
      }

      // XScore: if(!strcmp(atom.type,"N.2")&&atom.ring!=2)
      if (center_type == xtool_ff::N2 && !is_aromatic) {
        if (env.num_nonh == 1)
          assigned_type = xtool_ff::N2h;
        else if (env.num_nonh == 2)
          assigned_type = xtool_ff::N2;
        else
          assigned_type = xtool_ff::N2un;
      }

      // XScore: if(!strcmp(atom.type,"N.ar")||
      //            (!strcmp(atom.type,"N.2")&&atom.ring==2)||
      //            (!strcmp(atom.type,"N.pl3")&&atom.ring==2)||
      //            (!strcmp(atom.type,"N.am")&&atom.ring==2))
      if (center_type == xtool_ff::Nar || (center_type == xtool_ff::N2 && is_aromatic) ||
          (center_type == xtool_ff::Npl3 && is_aromatic) || (center_type == xtool_ff::Nam && is_aromatic)) {
        if (env.num_h == 1)
          assigned_type = xtool_ff::Narh;
        else if (env.num_h == 0)
          assigned_type = xtool_ff::Nar;
        else
          assigned_type = xtool_ff::Narun;
      }

      // XScore: if(!strcmp(atom.type,"N.1"))
      if (center_type == xtool_ff::N1) {
        if (env.num_nonh == 1)
          assigned_type = xtool_ff::N1;
        else
          assigned_type = xtool_ff::N1un;
      }

      // XScore: if(!strcmp(atom.type,"O.3"))
      if (center_type == xtool_ff::O3) {
        if (env.num_nonh == 1)
          assigned_type = xtool_ff::O3h;
        else if (env.num_nonh == 2)
          assigned_type = xtool_ff::O3;
        else
          assigned_type = xtool_ff::O3un;
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
        if (env.num_nonh == 1)
          assigned_type = xtool_ff::S3h;
        else if (env.num_nonh == 2)
          assigned_type = xtool_ff::S3;
        else
          assigned_type = xtool_ff::S3un;
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
        assigned_type = xtool_ff::So; // Note: XScore maps S.o2 → "S.o" (same as S.o)
      }

      // XScore: if(!strcmp(atom.type,"P.3"))
      if (center_type == xtool_ff::P3) {
        assigned_type = xtool_ff::P3;
      }

      // XScore: if(!strcmp(atom.type,"F"))
      if (center_type == xtool_ff::F)
        assigned_type = xtool_ff::F;

      // XScore: if(!strcmp(atom.type,"Cl"))
      if (center_type == xtool_ff::Cl)
        assigned_type = xtool_ff::Cl;

      // XScore: if(!strcmp(atom.type,"Br"))
      if (center_type == xtool_ff::Br)
        assigned_type = xtool_ff::Br;

      // XScore: if(!strcmp(atom.type,"I"))
      if (center_type == xtool_ff::I)
        assigned_type = xtool_ff::I;

      // XScore: if(!strcmp(atom.type,"Si"))
      if (center_type == xtool_ff::Si)
        assigned_type = xtool_ff::Si;

      // DATA SAVING
      layer.x_score_xtool_type(i) = assigned_type;
      layer.vdw_radius(i)         = get_description(assigned_type).vdw_radius;
      // Mirror XScore's Value_Atom: an atom that fails typing (unknown type) is
      // marked invalid, otherwise it is valid. Pocket promotion happens later in
      // define_pocket (protein only).
      layer.valid(i) =
          (assigned_type == xtool_ff::Un) ? x_score_validity::invalid : x_score_validity::valid;
    }
  }

  //===------------------------------------------------------------------------------------------------------
  // Converts old PDB hydrogen naming convention to modern PDB v3.
  // Old convention: digit prefix, e.g. "1HD2", "2HE2", "1HG1"
  // Modern convention: digit suffix, e.g. "HD21", "HE22", "HG11"
  // XScore's PDB parser handles both conventions; this normalization
  // allows our template lookup to match atoms from old-format PDB files.
  // Returns empty string if the name is not in old-convention format.
  //===------------------------------------------------------------------------------------------------------
  static std::string normalize_old_pdb_hydrogen_name(std::string_view name) {
    // Old convention: first char is a digit, remaining chars are the atom name
    // e.g. "1HD2" → leading '1' + "HD2" → "HD2" + '1' → "HD21"
    if (name.size() >= 2 && std::isdigit(static_cast<unsigned char>(name[0])) &&
        !std::isdigit(static_cast<unsigned char>(name[1]))) {
      std::string normalized(name.substr(1));
      normalized += name[0];
      return normalized;
    }
    return {};
  }

  // PROTEIN SPECIALIZATION
  template<>
  void assign_x_score_types(x_score_dynamic_layer& layer) {
    auto& mol = layer.get_base_molecule();
    // Implementation of residue-based lookup (Atom Name + Residue Name)
    // Writes directly to layer.atom_x_score_xtool_type[i]

    for (int i = 0; i < mol.num_atoms(); ++i) {
      residue res_type           = mol.residue_types(i);
      std::string_view atom_name = mol.atom_name(i);

      // Debug print to catch invalid residue types before they crash the program
      int res_enum_val = static_cast<int>(res_type);

      bool found = false;

      // if your residue in not UNKNOWN (27)
      if (res_enum_val >= 0 && res_enum_val < mudock::num_residues()) {
        const auto& res_desc = get_description(res_type);

        // Match the atom name to the dictionary entry
        for (const auto& atom_tmpl: res_desc.atoms) {
          if (atom_tmpl.name == atom_name) {
            // Assign the X-Score types found in the dictionary
            mol.atom_type(i)            = atom_tmpl.basic_atom_type;
            layer.x_score_xtool_type(i) = atom_tmpl.x_tool_atom_type;
            layer.vdw_radius(i)         = atom_tmpl.vdw_radius;
            //logp type can also be assigned here if needed
            found = true;
            break;
          }
        }

        // Fallback 1: try normalizing old PDB hydrogen names (e.g. "1HD2" → "HD21")
        // and retry the lookup in the same residue template.
        std::string normalized_name;
        if (!found) {
          normalized_name = normalize_old_pdb_hydrogen_name(atom_name);
          if (!normalized_name.empty()) {
            for (const auto& atom_tmpl: res_desc.atoms) {
              if (atom_tmpl.name == normalized_name) {
                mol.atom_type(i)            = atom_tmpl.basic_atom_type;
                layer.x_score_xtool_type(i) = atom_tmpl.x_tool_atom_type;
                layer.vdw_radius(i)         = atom_tmpl.vdw_radius;
                found                       = true;
                break;
              }
            }
          }
        }
      }

      if (found) {
        layer.valid(i) = x_score_validity::valid;
        continue;
      }
      //   std::cout << "!!! DEBUG WARNING !!! Invalid residue type detected.\n"
      //             << "Atom index: " << i << "\n"
      //             << "Atom name: " << atom_name << "\n"
      //             << "Raw residue enum value: " << res_enum_val << std::endl;
      //   continue; // Skip this atom and prevent the crash
      // } else {
      //   std::cout << std::left << "Atom name: " << std::setw(6) << atom_name
      //             << " Residue name: " << mudock::get_description(res_type).name << std::endl;
      // }

      // Fallback 2: check the TER (terminal) residue template.
      // Mirrors XScore behavior where, if an atom name is not found in its
      // own residue definition, the parser searches the TER template which
      // defines N- and C-terminal atoms (e.g., HN1, HN2, HN3, OXT, HOCA).
      if (!found) {
        const auto& ter_desc = get_description(residue::TER);
        // Try original name first
        for (const auto& atom_tmpl: ter_desc.atoms) {
          if (atom_tmpl.name == atom_name) {
            mol.atom_type(i)            = atom_tmpl.basic_atom_type;
            layer.x_score_xtool_type(i) = atom_tmpl.x_tool_atom_type;
            layer.vdw_radius(i)         = atom_tmpl.vdw_radius;
            found                       = true;
            break;
          }
        }
        // Try normalized name in TER as well
        std::string normalized_name;
        if (!found) {
          normalized_name = normalize_old_pdb_hydrogen_name(atom_name);
          if (!normalized_name.empty()) {
            for (const auto& atom_tmpl: ter_desc.atoms) {
              if (atom_tmpl.name == normalized_name) {
                mol.atom_type(i)            = atom_tmpl.basic_atom_type;
                layer.x_score_xtool_type(i) = atom_tmpl.x_tool_atom_type;
                layer.vdw_radius(i)         = atom_tmpl.vdw_radius;
                found                       = true;
                break;
              }
            }
          }
        }
      }

      // Fallback 3: check the HET (hetero) residue template.
      if (!found) {
        const auto& het_desc = get_description(residue::HET);
        // Try original name first
        for (const auto& atom_tmpl: het_desc.atoms) {
          if (atom_tmpl.name == atom_name) {
            mol.atom_type(i)            = atom_tmpl.basic_atom_type;
            layer.x_score_xtool_type(i) = atom_tmpl.x_tool_atom_type;
            layer.vdw_radius(i)         = atom_tmpl.vdw_radius;
            found                       = true;
            break;
          }
        }
        // Try normalized name in HET as well
        std::string normalized_name;
        if (!found) {
          normalized_name = normalize_old_pdb_hydrogen_name(atom_name);
          if (!normalized_name.empty()) {
            for (const auto& atom_tmpl: het_desc.atoms) {
              if (atom_tmpl.name == normalized_name) {
                mol.atom_type(i)            = atom_tmpl.basic_atom_type;
                layer.x_score_xtool_type(i) = atom_tmpl.x_tool_atom_type;
                layer.vdw_radius(i)         = atom_tmpl.vdw_radius;
                found                       = true;
                break;
              }
            }
          }
        }
      }

      if (!found) {
        // Handle cases where PDB name doesn't match any known template
        mol.atom_type(i)            = xtool_ff::Un;
        layer.x_score_xtool_type(i) = xtool_ff::Un;
        layer.vdw_radius(i)         = get_description(xtool_ff::Un).vdw_radius;
      }

      // Initial validity (XScore Value_Atom): valid if typed via a fallback
      // template, invalid if no template matched. Pocket promotion (valid==pocket)
      // is performed later by define_pocket.
      layer.valid(i) = found ? x_score_validity::valid : x_score_validity::invalid;

      // todo: debugging line shows atoms who's xtool type has been determined via fallback methods
      // std::string res_name_str = (res_enum_val >= 0 && res_enum_val < mudock::num_residues()) 
      //                            ? std::string(mudock::get_description(res_type).name) 
      //                            : "UNKNOWN";

      // std::cout << std::left 
      //           << "Atom name: "  << std::setw(6) << atom_name
      //           << " atom_type: " << std::setw(6) << mudock::get_description(mol.atom_type(i)).name
      //           << " Res_name: "  << std::setw(6) << res_name_str
      //           << " xtool_type: "<< std::setw(6) << mudock::get_description(layer.x_score_xtool_type(i)).name 
      //           << std::endl;


    }
  }

} // namespace mudock
