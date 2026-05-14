#pragma once

#include <functional>
#include <mudock/molecule.hpp>

namespace mudock {

  template<typename molecule_type>
    requires is_molecule<molecule_type>
  void assign_x_score_types(molecule_type&, std::function<void(molecule_type&)> = {});

  template<>
  void assign_x_score_types(static_molecule&, std::function<void(static_molecule&)>);
  template<>
  void assign_x_score_types(dynamic_molecule&, std::function<void(dynamic_molecule&)>);
} // namespace mudock
