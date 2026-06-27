#pragma once

namespace mudock {

  //===------------------------------------------------------------------------------------------------------
  // Per-atom validity state used by the X-Score scoring functions.
  //
  // Mirrors XScore's per-atom `int valid` field (legacy `score.cpp` / `protein.cpp`):
  //   0 -> invalid : atom failed typing/parametrization, or is a non-polar hydrogen
  //                  removed by Define_Pocket.
  //   1 -> valid   : atom successfully read and typed, but not part of the binding pocket.
  //   2 -> pocket  : protein atom belonging to the binding pocket (Define_Pocket).
  //
  // The underlying values are kept identical to XScore (0/1/2) so the ported scoring
  // gates read with the same semantics (e.g. ligand `valid != invalid`, protein
  // `valid == pocket`).
  //===------------------------------------------------------------------------------------------------------
  enum class x_score_validity : int { invalid = 0, valid = 1, pocket = 2 };

} // namespace mudock
