#pragma once

#include <cstddef>
#include <cstring>
#include <mudock/batch.hpp>
#include <mudock/chem/x_score_ligand.hpp>
#include <mudock/compute/x_score_kernel.hpp>
#include <mudock/chem/x_score_protein.hpp>
#include <mudock/chem/x_score_layer.hpp>
#ifndef __CUDACC__
  #include <mudock/compute/buffer_utils.hpp>
  #include <mudock/compute/scoring.hpp>
  #include <mudock/compute/scratchpad.hpp>
#endif
#include <mudock/molecule.hpp>
#include <mudock/type_alias.hpp>

//TODO: scratchpad to be understood

namespace mudock {

  template<typename queue_type>
  int get_x_score_batch(const int, std::shared_ptr<queue_type>, const size_t);

#ifndef __CUDACC__
  template<typename queue_type>
  struct x_score: public scoring<queue_type> {
    x_score(std::shared_ptr<scratchpad<queue_type>> _scratch,
            std::shared_ptr<scratchpad<queue_type>> _device_scratch,
            dynamic_molecule & protein)
        : scoring<queue_type>(_scratch),
          lig_vdw(_scratch->get_queue()),
          device_scratch(_device_scratch) {
            x_score_protein x_score_prot(protein);
      // Niente griglie/protein cache: non serve per vdw radius-only
    }

    void prepare(batch<static_molecule> &batch) {
      batch_ligands                = batch.num_ligands;
      batch_atoms                  = batch.batch_max_atoms;
      const int tot_atoms_in_batch = batch_ligands * batch_atoms;

      load_num_rotamers(batch, this->scratch);
      load_num_atoms(batch, this->scratch);

      const int scores_per_ligand =
          std::max(1, static_cast<int>((*this->scratch).configuration.population_number));

      auto &score_b = (*this->scratch).template get<buffer_data_type::SCORES>();
      if (!score_b.is_valid() ||
          score_b.num_elements() != static_cast<size_t>(batch_ligands * scores_per_ligand)) {
        score_b.alloc(batch_ligands * scores_per_ligand);
        score_b.set_valid();
      }

      // --- vdw radius per ligando ---
      lig_vdw.alloc(tot_atoms_in_batch);

      for (int ligand_index = 0; ligand_index < batch_ligands; ++ligand_index) {
        auto &ligand = *batch.molecules[ligand_index];
        const int stride_atoms = ligand_index * batch_atoms;
        const int num_atoms    = ligand.num_atoms();

        x_score_ligand xs_lig{ligand}; // prepara typing + vdw_radius
        const fp_type *vdw_src = xs_lig.vdw_radius_data(); // TODO: accessor nel layer

        std::memcpy(lig_vdw() + stride_atoms, vdw_src, num_atoms * sizeof(fp_type));
      }

      lig_vdw.copy_host2device();

      // Kernel minimale: usa solo vdw radius (e magari num_atoms/scores)
      const int *num_atoms_b = (*this->scratch).template get<buffer_data_type::NUM_ATOMS>().dev_pointer();
      fp_type *scores_b      = score_b.dev_pointer();
      const fp_type *lig_vdw_b = lig_vdw.dev_pointer();

      kernel = std::make_unique<x_score_kernel<queue_type>>(
          scores_per_ligand,
          batch_ligands,
          batch_atoms,
          num_atoms_b,
          lig_vdw_b,
          scores_b,
          (*this->scratch).get_queue());
    }

    void operator()() {
      assert(kernel && "Kernel method not yet prepared");
      (*kernel)();
    }

    static int get_ligand_mem(const int max_atoms, const knobs conf) {
      int mem{0};
      const int scores_per_ligand = std::max(1, static_cast<int>(conf.population_number));
      mem += sizeof(fp_type) * scores_per_ligand; // scores
      mem += sizeof(int);                         // num atoms
      mem += sizeof(fp_type) * max_atoms;         // vdw radius
      return mem;
    }

  private:
    int batch_ligands{0};
    int batch_atoms{0};

    buffer_vector<fp_type, queue_type> lig_vdw;
    std::shared_ptr<scratchpad<queue_type>> device_scratch;
    std::unique_ptr<x_score_kernel<queue_type>> kernel;

    void teardown_impl(batch<static_molecule> &batch) override {
      auto &scores_b               = (*this->scratch).template get<buffer_data_type::SCORES>();
      const auto scores_per_ligand = scores_b.num_elements() / batch.num_ligands;
      scores_b.copy_device2host();
      (*this->scratch).get_queue()->synchronize();
      for (int i = 0; i < batch.num_ligands; ++i) {
        auto &ligand = *batch.molecules[i];
        ligand.properties.assign(property_type::SCORE, std::to_string(scores_b()[i * scores_per_ligand]));
      }
    }
  };
#endif
} // namespace mudock