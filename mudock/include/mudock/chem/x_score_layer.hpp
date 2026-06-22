#pragma once

#include <mudock/chem/assign_autodock_types.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/chem/x_score_xlogp_types.hpp>
#include <mudock/chem/residue.hpp>
#include <mudock/chem/x_score_residue_xtool_types.hpp>
#include <mudock/chem/molecule_layer.hpp>
#include <mudock/grid/mdspan.hpp>
#include <mudock/type_alias.hpp>

namespace mudock {
  template<class container_aliases>
    requires is_container_specification<container_aliases>
  struct x_score_layer: public molecule_layer<container_aliases> {
    template<typename T>
    using atoms_array_type = container_aliases::template atoms_size<T>;
    // template<typename T>
    // using bonds_array_type = container_aliases::template bonds_size<T>;

    // data for the atoms



    x_score_layer(molecule<container_aliases>& _molecule) : molecule_layer<container_aliases>(_molecule) {
        // Trigger the preparation phase
        //resizing part
        const auto num_atoms = _molecule.num_atoms();

        mudock::resize(atom_x_score_xtool_type, num_atoms);
        mudock::resize(atom_x_score_xlogp_type, num_atoms);

        mudock::resize(atom_vdw_radius, num_atoms);
        // todo: add resize of other xscore parameters
        
        prepare();
    }

    // span
    [[nodiscard]] inline auto get_x_score_xtool_type() const {return make_span(atom_x_score_xtool_type, this->get_base_molecule().num_atoms());}
    [[nodiscard]] inline auto get_x_score_xlogp_type() const {return make_span(atom_x_score_xlogp_type, this->get_base_molecule().num_atoms());}
    [[nodiscard]] inline auto get_vdw_radius() const {return make_span(atom_vdw_radius, this->get_base_molecule().num_atoms());}

    // getter
    [[nodiscard]] inline auto& x_score_xtool_type(const int index) { return atom_x_score_xtool_type[index]; }
    [[nodiscard]] inline auto& x_score_xlogp_type(const int index) { return atom_x_score_xlogp_type[index]; }
    [[nodiscard]] inline auto& vdw_radius(const int index) { return atom_vdw_radius[index]; }

    // getter for pointer to the data
    [[nodiscard]] inline auto* x_score_xtool_type() { return atom_x_score_xtool_type.data(); }
    [[nodiscard]] inline auto* x_score_xlogp_type() { return atom_x_score_xlogp_type.data(); }
    [[nodiscard]] inline auto* vdw_radius() { return atom_vdw_radius.data(); }

    // getter for const reference
    [[nodiscard]] inline const auto& x_score_xtool_type(const int index) const { return atom_x_score_xtool_type[index]; }
    [[nodiscard]] inline const auto& x_score_xlogp_type(const int index) const { return atom_x_score_xlogp_type[index]; }
    [[nodiscard]] inline const auto& vdw_radius(const int index) const { return atom_vdw_radius[index]; }

    void resize(const int n_atoms, int n_bonds) {
      mudock::resize(atom_x_score_xtool_type, n_atoms);
      mudock::resize(atom_x_score_xlogp_type, n_atoms);
      mudock::resize(atom_vdw_radius, n_atoms);
      molecule<container_aliases>::resize(n_atoms, n_bonds);
    }

    void remove_atom(const int index) {
      mudock::remove_atom(atom_x_score_xtool_type, index);
      mudock::remove_atom(atom_x_score_xlogp_type, index);
      mudock::remove_atom(atom_vdw_radius, index);
      molecule<container_aliases>::remove_atom(index);
    }


  private:
    // todo: for the 3 structures below getters, setters and resize are to be implemented
    atoms_array_type<xtool_ff> atom_x_score_xtool_type;
    atoms_array_type<xlogp_ff> atom_x_score_xlogp_type;  

    // xtool radius
    atoms_array_type<fp_type> atom_vdw_radius; 
    //todo add other xlogp and xtool parameters as needed


    void assign_vdw_radii() {
      const auto num_atoms = this->get_base_molecule().num_atoms();
      for (int i = 0; i < num_atoms; ++i) {
        atom_vdw_radius[i] = get_description(atom_x_score_xtool_type[i]).vdw_radius;
      }
    }

    // declared as virtual, implemented in subclasses
    void prepare() {
      // 1. Assign xtool and xlogp types based on the residue type of each atom
      assign_x_score_types((*this));
      // 2. Assign vdw radii
      assign_vdw_radii();
    }
  
  
  
  
  };

  using x_score_dynamic_layer = x_score_layer<dynamic_containers>;
  using x_score_static_layer  = x_score_layer<static_containers>;

  template<class T>
  concept is_x_score_layer = (std::same_as<std::remove_cvref_t<T>, x_score_static_layer> ||
                              std::same_as<std::remove_cvref_t<T>, x_score_dynamic_layer>);
  template<class T>
  concept derived_from_x_score_layer =
      (std::derived_from<T, x_score_static_layer> || std::derived_from<T, x_score_dynamic_layer>);
} // namespace mudock
