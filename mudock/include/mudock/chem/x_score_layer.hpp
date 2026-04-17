#pragma once

#include <mudock/chem/assign_autodock_types.hpp>
#include <mudock/chem/autodock_babel_types.hpp>
#include <mudock/chem/autodock_grid_types.hpp>
#include <mudock/chem/autodock_parameters.hpp>
#include <mudock/chem/molecule_layer.hpp>
#include <mudock/grid/mdspan.hpp>
#include <mudock/type_alias.hpp>

namespace mudock {
  template<class container_aliases>
    requires is_container_specification<container_aliases>
  struct x_score_layer: public molecule_layer<container_aliases> {
    template<typename T>
    using atoms_array_type = container_aliases::template atoms_size<T>;
    template<typename T>
    using bonds_array_type = container_aliases::template bonds_size<T>;

    x_score_layer(molecule<container_aliases>& _molecule,
                   std::function<void(molecule<container_aliases>&)> f = {})
        : molecule_layer<container_aliases>(_molecule) {

    };
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
