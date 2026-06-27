#pragma once

#include <mudock/chem/x_score_ligand.hpp>
#include <mudock/chem/x_score_protein.hpp>
#include <mudock/type_alias.hpp>

namespace mudock {

  /**
   * @brief Define the binding pocket on a protein, porting XScore's
   *        Protein::Define_Pocket (protein.cpp:1338-1450).
   *
   * Promotes protein atoms to x_score_validity::pocket so that scoring functions
   * (e.g. the VDW term) only consider binding-site atoms. The procedure:
   *   1. Remove non-polar hydrogens (unit-atom model) by marking them invalid.
   *   2. Mark every protein atom within @p cutoff of a valid, non-hydrogen ligand
   *      atom as a pocket atom, collecting the distinct pocket residues
   *      (keyed by residue type + sequence id + chain), excluding waters/metals.
   *   3. Require at least 3 amino-acid pocket residues, else throw (the complex is
   *      probably not docked); mirrors XScore's fatal check.
   *   4. Expand the pocket to every remaining atom belonging to a pocket residue.
   *
   * The reference-less form is used (the ligand itself defines the pocket), matching
   * XScore's Xtool_Score_Shortcut path and our "ignore the mol2 reference" requirement.
   *
   * Note: XScore's Define_Pocket additionally pre-computes aromatic rings within the
   * pocket residues (protein.cpp:1452-1496) for pi-stacking scoring; that step does not
   * affect atom validity and is intentionally not ported here.
   *
   * @param protein The protein layer whose atom validity is updated in place.
   * @param ligand  The ligand defining the binding site.
   * @param cutoff  Distance cutoff in Angstrom (XScore default: 10.0).
   */
  void define_pocket(x_score_protein& protein, const x_score_ligand& ligand, fp_type cutoff = fp_type{10});

} // namespace mudock
