#pragma once

#include <concepts>
#include <memory>
#include <mudock/compute/queue.hpp>
#include <mudock/type_alias.hpp>

namespace mudock {
  // X-Score scoring kernel. For now it implements the van der Waals (vdw) term only.
  //
  // The kernel mirrors the adt_score_kernel design: it stores raw (device) pointers to
  // the data prepared by the x_score stage, and the actual computation is provided per
  // backend through the explicit specialization of operator() (e.g. queue_cpp in
  // x_score_cpp.cpp).
  //
  // The protein atom arrays are constant across the whole batch (a single target), while
  // the ligand atom arrays are laid out per-ligand with a stride of batch_atoms.
  template<typename queue_type>
    requires std::derived_from<queue_type, queue>
  struct x_score_kernel {
    static constexpr char x_region_name[] = "x_score_kernel";

    x_score_kernel(const int scores_per_ligand_,
                   const int batch_ligands_,
                   const int batch_atoms_,
                   const int *__restrict__ num_atoms_b_,
                   const fp_type *__restrict__ lig_x_b_,
                   const fp_type *__restrict__ lig_y_b_,
                   const fp_type *__restrict__ lig_z_b_,
                   const fp_type *__restrict__ lig_vdw_b_,
                   const int *__restrict__ lig_scorable_b_,
                   const int num_prot_atoms_,
                   const fp_type *__restrict__ prot_x_b_,
                   const fp_type *__restrict__ prot_y_b_,
                   const fp_type *__restrict__ prot_z_b_,
                   const fp_type *__restrict__ prot_vdw_b_,
                   const int *__restrict__ prot_scorable_b_,
                   fp_type *__restrict__ scores_b_,
                   std::shared_ptr<queue_type> q_)
        : scores_per_ligand(scores_per_ligand_),
          batch_ligands(batch_ligands_),
          batch_atoms(batch_atoms_),
          num_atoms_b(num_atoms_b_),
          lig_x_b(lig_x_b_),
          lig_y_b(lig_y_b_),
          lig_z_b(lig_z_b_),
          lig_vdw_b(lig_vdw_b_),
          lig_scorable_b(lig_scorable_b_),
          num_prot_atoms(num_prot_atoms_),
          prot_x_b(prot_x_b_),
          prot_y_b(prot_y_b_),
          prot_z_b(prot_z_b_),
          prot_vdw_b(prot_vdw_b_),
          prot_scorable_b(prot_scorable_b_),
          scores_b(scores_b_),
          q(q_) {}

    void operator()();

    x_score_kernel(const x_score_kernel &)            = default;
    x_score_kernel(x_score_kernel &&)                 = default;
    x_score_kernel &operator=(const x_score_kernel &) = delete;
    x_score_kernel &operator=(x_score_kernel &&)      = delete;

    ~x_score_kernel() = default;

  private:
    const int scores_per_ligand;
    const int batch_ligands;
    const int batch_atoms;
    const int *__restrict__ num_atoms_b;

    // ligand atom data, strided per ligand by batch_atoms
    const fp_type *__restrict__ lig_x_b;
    const fp_type *__restrict__ lig_y_b;
    const fp_type *__restrict__ lig_z_b;
    const fp_type *__restrict__ lig_vdw_b;
    const int *__restrict__ lig_scorable_b;

    // protein atom data, shared across the whole batch (single target)
    const int num_prot_atoms;
    const fp_type *__restrict__ prot_x_b;
    const fp_type *__restrict__ prot_y_b;
    const fp_type *__restrict__ prot_z_b;
    const fp_type *__restrict__ prot_vdw_b;
    const int *__restrict__ prot_scorable_b;

    fp_type *__restrict__ scores_b;
    std::shared_ptr<queue_type> q;
  };

} // namespace mudock
