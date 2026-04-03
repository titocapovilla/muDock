#pragma once

#include <cstddef>
#include <cstring>
#include <mudock/batch.hpp>
#include <mudock/chem/autodock_grid_types.hpp>
#include <mudock/chem/autodock_ligand.hpp>
#include <mudock/chem/autodock_protein.hpp>
#include <mudock/compute/adt_score_kernel.hpp>
#ifndef __CUDACC__
  #include <mudock/compute/buffer_utils.hpp>
  #include <mudock/compute/scoring.hpp>
  #include <mudock/compute/scratchpad.hpp>
#endif
#include <mudock/molecule.hpp>
#include <mudock/type_alias.hpp>

namespace mudock {

  template<typename queue_type>
  // requires std::derived_from<queue_type, queue>
  int get_x_score_batch(const int, std::shared_ptr<queue_type>, const size_t);

#ifndef __CUDACC__
  // TODO check that the object type and the kernel impl are the same
  template<typename queue_type>
  struct x_score: public scoring<queue_type> {
    x_score(std::shared_ptr<scratchpad<queue_type>> _scratch,
              std::shared_ptr<scratchpad<queue_type>> _device_scratch,
              dynamic_molecule &protein)
        : scoring<queue_type>(_scratch),
          vols(_scratch->get_queue()),
          solpars(_scratch->get_queue()),
          charges(_scratch->get_queue()),
          map_offsets(_scratch->get_queue()),
          num_nonbond(_scratch->get_queue()),
          nonbond_a1(_scratch->get_queue()),
          nonbond_a2(_scratch->get_queue()),
          nonbond_cA(_scratch->get_queue()),
          nonbond_cB(_scratch->get_queue()),
          nonbond_xB(_scratch->get_queue()),
          device_scratch(_device_scratch) {

            // TODO implement constructor

    }

    void prepare(batch<static_molecule> &batch) {

      // TODO implement Value_Atom()

      // TODO kernel call to be filled with correct parameters
      kernel = std::make_unique<x_score_kernel<queue_type>>(scores_per_ligand,
                                                              batch_ligands,
                                                              batch_atoms,
                                                              num_atoms_b,
                                                              num_rotamers_b,
                                                              num_nonbonds_b,
                                                              x_scratch_b,
                                                              y_scratch_b,
                                                              z_scratch_b,
                                                              vols_b,
                                                              solpars_b,
                                                              charges_b,
                                                              map_offsets_b,
                                                              nonbond_a1_b,
                                                              nonbond_a2_b,
                                                              nonbond_cA_b,
                                                              nonbond_cB_b,
                                                              nonbond_xB_b,
                                                              grid_maps,
                                                              minimum,
                                                              maximum,
                                                              center,
                                                              map_index_x,
                                                              map_index_xy,
                                                              map_index_xyz,
                                                              scores_b,
                                                              q);
    }

    void operator()() {
      assert(
          (((*this->scratch).template get<buffer_data_type::SCORES>().num_elements() % batch_ligands) == 0) &&
          "Number of scores is not a multiple of ligands in the batch");
      assert(kernel && "Kernel method not yet prepared");
      (*kernel)();
    }

  private:
  // TODO preserve only useful parameters
    int batch_ligands;
    int batch_atoms;

    buffer_vector<fp_type, queue_type> vols;
    buffer_vector<fp_type, queue_type> solpars;
    buffer_vector<fp_type, queue_type> charges;
    buffer_vector<int, queue_type> map_offsets;
    buffer_vector<int, queue_type> num_nonbond;
    buffer_vector<int, queue_type> nonbond_a1;
    buffer_vector<int, queue_type> nonbond_a2;
    buffer_vector<fp_type, queue_type> nonbond_cA;
    buffer_vector<fp_type, queue_type> nonbond_cB;
    buffer_vector<int, queue_type> nonbond_xB;

    std::shared_ptr<scratchpad<queue_type>> device_scratch;
    std::unique_ptr<x_score_kernel<queue_type>> kernel;

    void teardown_impl(batch<static_molecule> &batch) override {

    };
  };
#endif
} // namespace mudock
