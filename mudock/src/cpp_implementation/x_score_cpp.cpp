#include <cstring>
#include <mudock/chem/autodock_ligand.hpp>
#include <mudock/chem/mehler_solmajer.hpp>
#include <mudock/cpp_implementation/x_score_cpp.hpp>

#define FLATTENED_2D(x, y, index_x)              ((y) * index_x + (x))
#define FLATTENED_3D(x, y, z, index_x, index_xy) (index_xy * (z) + (y) * index_x + (x))

namespace mudock {
  inline fp_type trilinear_interpolation(const fp_type *__restrict__ map,
                                         const fp_type *__restrict__ coeffs,
                                         const int &map_index_x,
                                         const int &map_index_xy) {
    fp_type value{0};

    value = coeffs[0] * map[0] + value;
    value = coeffs[1] * map[map_index_xy] + value;
    value = coeffs[2] * map[map_index_x] + value;
    value = coeffs[3] * map[map_index_x + map_index_xy] + value;
    value = coeffs[4] * map[1] + value;
    value = coeffs[5] * map[1 + map_index_xy] + value;
    value = coeffs[6] * map[1 + map_index_x] + value;
    value = coeffs[7] * map[1 + map_index_x + map_index_xy] + value;

    return value;
  }

  inline void calc_energy(const int batch_atoms,
                          const int batch_ligands,
                          const int scores_per_ligand,
                          const fp_type *__restrict__ x_scratch_b,
                          const fp_type *__restrict__ y_scratch_b,
                          const fp_type *__restrict__ z_scratch_b,
                          const fp_type *__restrict__ vols_b,
                          const fp_type *__restrict__ solpars_b,
                          const fp_type *__restrict__ charges_b,
                          const int *__restrict__ num_atoms_b,
                          const int *__restrict__ num_rotamers_b,
                          const int *__restrict__ num_nonbonds_b,
                          const int *__restrict__ nonbond_a1_b,
                          const int *__restrict__ nonbond_a2_b,
                          const fp_type *__restrict__ nonbond_cA_b,
                          const fp_type *__restrict__ nonbond_cB_b,
                          const int *__restrict__ nonbond_xB_b,
                          const fp_type *__restrict__ grid_maps,
                          const fp_type *__restrict__ minimum,
                          const fp_type *__restrict__ maximum,
                          const fp_type *__restrict__ center,
                          const int *__restrict__ map_offsets_b,
                          const int map_index_x,
                          const int map_index_xy,
                          const int map_index_xyz,
                          fp_type *__restrict__ scores_b) {

  };

  template<>
  void x_score_kernel<queue_cpp>::operator()() {
    q->invoke_kernel<x_score_kernel::x_region_name>(calc_energy,
                                            batch_atoms,
                                            batch_ligands,
                                            scores_per_ligand,
                                            x_scratch_b,
                                            y_scratch_b,
                                            z_scratch_b,
                                            vols_b,
                                            solpars_b,
                                            charges_b,
                                            num_atoms_b,
                                            num_rotamers_b,
                                            num_nonbonds_b,
                                            nonbond_a1_b,
                                            nonbond_a2_b,
                                            nonbond_cA_b,
                                            nonbond_cB_b,
                                            nonbond_xB_b,
                                            grid_maps,
                                            minimum,
                                            maximum,
                                            center,
                                            map_offsets_b,
                                            map_index_x,
                                            map_index_xy,
                                            map_index_xyz,
                                            scores_b);
  }
} // namespace mudock
