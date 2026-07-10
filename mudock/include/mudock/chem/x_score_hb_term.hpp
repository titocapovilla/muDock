#pragma once

#include <mudock/chem/x_score_ligand.hpp>
#include <mudock/chem/x_score_protein.hpp>
#include <mudock/type_alias.hpp>
#include <vector>

namespace mudock {

  // Hydrogen-bond (HB) term of the X-Score scoring function, ported from Ligand::Calculate_HB,
  // Get_HBond_Pair_PL, Sum_HBonds and HBond::Value_HBond_2 (score.cpp / basic.cpp).

  struct x_score_hb_atom {
    fp_type x{0}, y{0}, z{0};    // atom coordinates
    fp_type rx{0}, ry{0}, rz{0}; // H-bond "root" (average of heavy neighbours)
    fp_type radius{0};           // X-Tool vdw radius
    int hb{0};                   // hydrogen-bonding class (x_score_hb code)
    int donor_type{0};           // XScore Get_Donor_Type (0 none / 1 straight / 2 angled)
    int acceptor_type{0};        // XScore Get_Acceptor_Type (0 none / 1 straight / 2 angled)
    bool is_metal{false};        // hb == M (skips the donor angle)
    bool is_water{false};        // basic type O.w (skips the relevant angle)
    bool has_root{false};        // whether a heavy-neighbour root could be computed
    int num_h{0};                // bonded hydrogens (ligand donor saturation limit)
    char element{'X'};           // 'O'/'N'/'S'/other (ligand acceptor lone-pair limit)
  };

  // Build the H-bond atom list for the protein target.
  [[nodiscard]] std::vector<x_score_hb_atom> build_protein_hb_atoms(const x_score_protein& prot);

  // Build the H-bond atom list for a single ligand pose.
  [[nodiscard]] std::vector<x_score_hb_atom> build_ligand_hb_atoms(const x_score_ligand& lig);

  // Score all protein-ligand H-bonds and return the HB term for this ligand pose.
  [[nodiscard]] fp_type compute_x_score_hb(const std::vector<x_score_hb_atom>& lig_atoms,
                                           const std::vector<x_score_hb_atom>& prot_atoms);

} // namespace mudock