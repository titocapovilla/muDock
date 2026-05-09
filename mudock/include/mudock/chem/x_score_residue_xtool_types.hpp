#pragma once

#include <array>
#include <cassert>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/type_alias.hpp>
#include <span>
#include <string_view>
#include <unordered_map>

//===------------------------------------------------------------------------------------------------------
// WARNING: This file has been automatically generated from chem/x_score_residue_xtool_types.json
//===------------------------------------------------------------------------------------------------------

namespace mudock {

  // List of all known residues for X-TOOL force field
  enum class xtool_residue : int {

    ACE = 0, // Acetyl

    ALA = 1, // Alanine

    ARG = 2, // Arginine

    ASN = 3, // Asparagine

    ASP = 4, // Aspartic acid

    CYS = 5, // Cysteine

    GLN = 6, // Glutamine

    GLU = 7, // Glutamic acid

    GLY = 8, // Glycine

    HIS = 9, // Histidine (all variations)

    ILE = 10, // Isoleucine

    LEU = 11, // Leucine

    LYS = 12, // Lysine

    MET = 13, // Methionine

    NME = 14, // N-Methyl

    PHE = 15, // Phenylalanine

    PRO = 16, // Proline

    SER = 17, // Serine

    THR = 18, // Threonine

    TRP = 19, // Tryptophan

    TYR = 20, // Tyrosine

    VAL = 21, // Valine

    TER = 22, // N- and C-terminal atoms

    HOH = 23, // Water

    SO4 = 24, // SO4--

    PO4 = 25, // PO4--

    HET = 26, // ions

  };

  // Description of an atom within a residue
  struct xtool_residue_atom_description {
    std::string_view name;
    xtool_ff basic_atom_type;
    xtool_ff x_tool_atom_type;
    fp_type vdw_radius;
    fp_type vdw_potential;
    fp_type par_charge;
    std::string_view hbond;
    fp_type hydrophobic_scale;
    fp_type sas_parameter;
    int ring_indicator;
    std::string_view pmf_atom_type;
  };

  // Description of a bond within a residue
  struct xtool_residue_bond_description {
    std::string_view atom_1;
    std::string_view atom_2;
    int bond_type;
  };

  // Description of a residue
  struct xtool_residue_description {
    xtool_residue value;
    std::string_view name;
    fp_type total_charge;
    std::string_view description;
    std::span<const xtool_residue_atom_description> atoms;
    std::span<const xtool_residue_bond_description> bonds;
  };

  static constexpr auto num_xtool_residues() { return 27; }
  extern const std::array<xtool_residue_description, num_xtool_residues()> XTOOL_RESIDUE_DICTIONARY;
  extern const std::unordered_map<std::string_view, xtool_residue> XTOOL_RESIDUE_LOOKUP;

  // Utility function to get the description by enum
  inline const xtool_residue_description& get_description(const xtool_residue r) {
    assert(static_cast<int>(r) >= 0 && static_cast<int>(r) < num_xtool_residues());
    assert(XTOOL_RESIDUE_DICTIONARY[static_cast<int>(r)].value == r);
    return XTOOL_RESIDUE_DICTIONARY[static_cast<int>(r)];
  }

  // Utility function to get the residue by name (e.g., "ALA")
  inline const xtool_residue_description* get_residue_by_name(const std::string_view name) {
    if (auto it = XTOOL_RESIDUE_LOOKUP.find(name); it != XTOOL_RESIDUE_LOOKUP.end()) {
      return &get_description(it->second);
    }
    return nullptr;
  }

} // namespace mudock