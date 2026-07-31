#pragma once

#include <mudock/chem/x_score_hb.hpp>
#include <mudock/chem/x_score_ligand.hpp>
#include <mudock/chem/x_score_protein.hpp>
#include <mudock/type_alias.hpp>
#include <vector>

namespace mudock {

  // Hydrogen-bond (HB) term of the X-Score scoring function, ported from Ligand::Calculate_HB,
  // Get_HBond_Pair_PL, Sum_HBonds and HBond::Value_HBond_2 (score.cpp / basic.cpp).

  struct x_score_hb_atoms {
    std::vector<fp_type> x, y, z;                // atom coordinates
    std::vector<fp_type> root_x, root_y, root_z; // H-bond "root" (average of heavy neighbours)
    std::vector<fp_type> radius;                 // X-Tool vdw radius
    std::vector<x_score_hb> hb;                  // hydrogen-bonding class
    std::vector<int> donor_type;                 // XScore Get_Donor_Type (0 none / 1 straight / 2 angled)
    std::vector<int> acceptor_type;              // XScore Get_Acceptor_Type (same encoding)
    std::vector<int> donor_limit;                // Sum_HBonds step 3 saturation limit
    std::vector<int> acceptor_limit;             // Sum_HBonds step 4 lone-pair limit
    std::vector<char> has_root;                  // whether a heavy-neighbour root could be computed

    [[nodiscard]] int size() const { return static_cast<int>(x.size()); }
  };

  // Step 1: build the H-bond atom data for the protein target.
  [[nodiscard]] x_score_hb_atoms build_protein_hb_atoms(const x_score_protein& prot);

  // Step 1: build the H-bond atom data for a single ligand pose.
  [[nodiscard]] x_score_hb_atoms build_ligand_hb_atoms(const x_score_ligand& lig);

  // One candidate H-bond between a ligand atom and a protein atom (XScore's HBond record).
  struct x_score_hb_candidate {
    int li;       // entry index into the ligand x_score_hb_atoms
    int pj;       // entry index into the protein x_score_hb_atoms
    int type;     // 1: lig donor; 2: lig acceptor / prot donor; 3: lig acceptor / metal
    fp_type score;
  };

  // Step 2 (Get_HBond_Pair_PL): every ligand/protein pair that can form an H-bond, scored
  // geometrically. Returns the unfiltered candidate list.
  [[nodiscard]] std::vector<x_score_hb_candidate> get_hbond_pair_pl(const x_score_hb_atoms& lig_atoms,
                                                                    const x_score_hb_atoms& prot_atoms);

  // Step 3 (Sum_HBonds + the summation in Calculate_HB): rank the candidates, drop the ones that
  // violate the angular and saturation rules, and sum what survives. `candidates` is modified in
  // place, mirroring XScore which zeroes the score of a rejected candidate.
  [[nodiscard]] fp_type sum_hbonds(std::vector<x_score_hb_candidate>& candidates,
                                   const x_score_hb_atoms& lig_atoms,
                                   const x_score_hb_atoms& prot_atoms);

  // Steps 2 and 3 together: the HB term for this ligand pose.
  [[nodiscard]] fp_type compute_x_score_hb(const x_score_hb_atoms& lig_atoms,
                                           const x_score_hb_atoms& prot_atoms);

} // namespace mudock