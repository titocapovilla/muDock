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
  // gates read with the same semantics.
  //
  // `pocket` is never assigned here: XScore's Define_Pocket is not ported, so the scoring
  // gates test `valid != invalid` where XScore tested `valid == 2` (score.cpp:404, 669, 909).
  // The two agree on every atom that can contribute, because the scoring cutoffs (5 A for HB,
  // 8 A for VDW/HP) are all stricter than XScore's 10 A pocket radius. The enumerator is kept
  // so the value mapping to XScore stays readable.
  //===------------------------------------------------------------------------------------------------------
  enum class x_score_validity : int { invalid = 0, valid = 1, pocket = 2 };

} // namespace mudock
