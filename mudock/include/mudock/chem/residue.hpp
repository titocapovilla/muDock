#pragma once

#include <array>
#include <cassert>
#include <mudock/type_alias.hpp>
#include <span>
#include <string_view>
#include <unordered_map>

namespace mudock {

  // List of all known residues 
  enum class residue : int {

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

    UNKNOWN = 27

  };

  static constexpr auto num_xtool_residues() { return 27; }

} // namespace mudock