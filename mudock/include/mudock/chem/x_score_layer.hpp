#pragma once

#include <mudock/chem/assign_autodock_types.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/chem/x_score_xlogp_types.hpp>
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



    x_score_layer(molecule<container_aliases>& _molecule,
                  std::function<void(molecule<container_aliases>&)> f = {})
       : molecule_layer<container_aliases>(_molecule) {
        // Trigger the preparation phase
        //resizing part
        const auto num_atoms = _molecule.num_atoms();
        mudock::resize(vdw_radius, n_atoms);
        //resize others
        
        
        // 2. Decide which engine to use
        // if (mol.properties.get("type") == "PROTEIN") {
        //     prepare_protein(); // Uses the Residue Dictionary
        // } else {
        //     prepare_ligand();  // Uses the molecule.cpp logic
        // }

        prepare(f); 
    }

  private:
    // xtool type array
    atoms_array_type<xtool_ff>   atom_xtool_type;  
    // todo add xlogp type

    // xtool radius
    atoms_array_type<fp_type>    vdw_radius; 
    //todo add other xlogp and xtool parameters as needed

    
  // Atom Typing: For every atom in the molecule, determine its xtool_ff and xlogp_ff identity.
  // Parameter Extraction: Look up that identity in your XTOOL_FF_DICTIONARY and XLOGP_FF_DICTIONARY.
  // Filling: Copy the radius, potential, charge, and scales into the layer's contiguous arrays.

  void prepare(std::function<void(molecule<container_aliases>&)> f = {}) {
      //assign_x_score_xtool_types((*this)(), f);
      //assign_x_score_xlogp_types((*this)(), f);


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
