#pragma once

#include <string_view>

namespace mudock {

  // Per-atom hydrogen-bonding character used by the X-Score scoring functions.
  //
  // Mirrors the 7th column of XScore's ATOM_DEF_XTOOL / RESIDUE_DEF_XTOOL dictionaries
  //   N  -> none           : atom plays no specific role (e.g. non-polar hydrogen)
  //   H  -> hydrophobic     : hydrophobic atom (drives the HP / HM / HS terms)
  //   P  -> polar           : polar atom that is neither donor nor acceptor
  //   D  -> donor           : H-bond donor
  //   A  -> acceptor        : H-bond acceptor
  //   DA -> donor/acceptor  : both donor and acceptor
  //   DH -> polar hydrogen   : polar hydrogen attached to a donor
  //   M  -> metal            : metal ion (acts as an acceptor partner in HB)

  enum class x_score_hb : int { N = 0, H = 1, P = 2, D = 3, A = 4, DA = 5, DH = 6, M = 7 };


} // namespace mudock