#pragma once

#include <mudock/chem/assign_autodock_types.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/chem/x_score_xlogp_types.hpp>
#include <mudock/chem/residue.hpp>
#include <mudock/chem/x_score_xtool_residue_types.hpp>
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

        mudock::resize(vdw_radius, num_atoms);
        // todo: add resize of other xscore parameters
        
        //prepare() is called in the specialized classes
    }

  protected:
    // todo: for the 3 structures below getters, setters and resize are to be implemented
    atoms_array_type<xtool_ff> atom_x_score_xtool_type;
    atoms_array_type<xlogp_ff> atom_x_score_xlogp_type;  

    // xtool radius
    atoms_array_type<fp_type> vdw_radius; 
    //todo add other xlogp and xtool parameters as needed

    // declared as virtual, implemented in subclasses
    virtual void prepare() = 0;
  
  
  
  
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
