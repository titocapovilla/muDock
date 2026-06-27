#include <cmath>
#include <mudock/cpp_implementation/x_score_cpp.hpp>

namespace mudock {

  // XScore vdw distance cutoff: pairs farther apart than this do not contribute.
  static constexpr fp_type x_score_dist_cutoff = fp_type{8.0};

  // Van der Waals term ported from XScore (Calculate_VDW, score.cpp) and from the
  // reference prototype in test_x_score_types.cpp::calculate_vdw.
  //
  // For each scorable ligand atom we accumulate, over the scorable protein atoms within
  // the distance cutoff, the (d0/d)^12 - 2*(d0/d)^6 well shape (expressed as ((d0/d)^4)^2
  // - 2*(d0/d)^4), flip its sign so that favorable interactions are positive, and discard
  // per-atom contributions that turn out unfavorable. The ligand score is the sum of the
  // surviving per-atom contributions.
  inline void calc_x_score(const int batch_atoms,
                           const int batch_ligands,
                           const int scores_per_ligand,
                           const int *__restrict__ num_atoms_b,
                           const fp_type *__restrict__ lig_x_b,
                           const fp_type *__restrict__ lig_y_b,
                           const fp_type *__restrict__ lig_z_b,
                           const fp_type *__restrict__ lig_vdw_b,
                           const int *__restrict__ lig_scorable_b,
                           const int num_prot_atoms,
                           const fp_type *__restrict__ prot_x_b,
                           const fp_type *__restrict__ prot_y_b,
                           const fp_type *__restrict__ prot_z_b,
                           const fp_type *__restrict__ prot_vdw_b,
                           const int *__restrict__ prot_scorable_b,
                           fp_type *__restrict__ scores_b) {
    for (int ligand_index = 0; ligand_index < batch_ligands; ++ligand_index) {
      const int atom_stride = ligand_index * batch_atoms;
      const int num_atoms   = num_atoms_b[ligand_index];

      const fp_type *__restrict__ lig_x   = lig_x_b + atom_stride;
      const fp_type *__restrict__ lig_y   = lig_y_b + atom_stride;
      const fp_type *__restrict__ lig_z   = lig_z_b + atom_stride;
      const fp_type *__restrict__ lig_vdw = lig_vdw_b + atom_stride;
      const int *__restrict__ lig_scorable = lig_scorable_b + atom_stride;

      fp_type sum = 0;
      for (int i = 0; i < num_atoms; ++i) {
        if (!lig_scorable[i])
          continue;

        const fp_type lx = lig_x[i];
        const fp_type ly = lig_y[i];
        const fp_type lz = lig_z[i];
        const fp_type lr = lig_vdw[i];

        fp_type asum = 0;
        for (int j = 0; j < num_prot_atoms; ++j) {
          if (!prot_scorable_b[j])
            continue;

          const fp_type dx = lx - prot_x_b[j];
          const fp_type dy = ly - prot_y_b[j];
          const fp_type dz = lz - prot_z_b[j];
          const fp_type d  = std::sqrt(dx * dx + dy * dy + dz * dz);

          if (d > x_score_dist_cutoff)
            continue;

          const fp_type d0   = lr + prot_vdw_b[j];
          fp_type tmp1       = d0 / d;
          tmp1               = tmp1 * tmp1 * tmp1 * tmp1; // (d0/d)^4
          const fp_type tmp2 = tmp1 * tmp1;               // (d0/d)^8
          asum += tmp2 - fp_type{2} * tmp1;
        }

        asum *= fp_type{-1}; // favorable interactions become positive

        if (asum < fp_type{0})
          continue; // discard unfavorable per-atom contributions
        sum += asum;
      }

      fp_type *__restrict__ scores_l = scores_b + ligand_index * scores_per_ligand;
      for (int s = 0; s < scores_per_ligand; ++s)
        scores_l[s] = sum;
    }
  };

  template<>
  void x_score_kernel<queue_cpp>::operator()() {
    q->invoke_kernel<x_score_kernel::x_region_name>(calc_x_score,
                                                    batch_atoms,
                                                    batch_ligands,
                                                    scores_per_ligand,
                                                    num_atoms_b,
                                                    lig_x_b,
                                                    lig_y_b,
                                                    lig_z_b,
                                                    lig_vdw_b,
                                                    lig_scorable_b,
                                                    num_prot_atoms,
                                                    prot_x_b,
                                                    prot_y_b,
                                                    prot_z_b,
                                                    prot_vdw_b,
                                                    prot_scorable_b,
                                                    scores_b);
  }
} // namespace mudock
