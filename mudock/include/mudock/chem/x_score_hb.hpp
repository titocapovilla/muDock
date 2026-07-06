#pragma once

#include <string_view>

namespace mudock {

  // Per-atom hydrogen-bonding character used by the X-Score scoring functions.
  //
  // Mirrors the 7th column of XScore's ATOM_DEF_XTOOL / RESIDUE_DEF_XTOOL dictionaries
  // (legacy `atom.hb` string field):
  //   N  -> none           : atom plays no specific role (e.g. non-polar hydrogen)
  //   H  -> hydrophobic     : hydrophobic atom (drives the HP / HM / HS terms)
  //   P  -> polar           : polar atom that is neither donor nor acceptor
  //   D  -> donor           : H-bond donor
  //   A  -> acceptor        : H-bond acceptor
  //   DA -> donor/acceptor  : both donor and acceptor
  //   DH -> polar hydrogen   : polar hydrogen attached to a donor
  //   M  -> metal            : metal ion (acts as an acceptor partner in HB)
  //
  // The classification is authoritative per source: ligand atoms read it from the X-Tool
  // atom dictionary (xtool_ff_description::hbond), protein atoms from the residue dictionary
  // (xtool_residue_atom_description::hbond). The two can disagree on purpose (e.g. O.co2 is
  // DA on the ligand but A on the protein), which is why it is stored per atom rather than
  // re-derived from the xtool type at scoring time.

  enum class x_score_hb : int { N = 0, H = 1, P = 2, D = 3, A = 4, DA = 5, DH = 6, M = 7 };

  // Parse the XScore hydrogen-bonding token ("N"/"H"/"P"/"D"/"A"/"DA"/"DH"/"M") into the enum.
  // Unknown / empty tokens map to N (none), matching XScore's neutral default.
  inline x_score_hb parse_x_score_hb(const std::string_view token) {
    if (token == "H")
      return x_score_hb::H;
    if (token == "P")
      return x_score_hb::P;
    if (token == "D")
      return x_score_hb::D;
    if (token == "A")
      return x_score_hb::A;
    if (token == "DA")
      return x_score_hb::DA;
    if (token == "DH")
      return x_score_hb::DH;
    if (token == "M")
      return x_score_hb::M;
    return x_score_hb::N;
  }

} // namespace mudock