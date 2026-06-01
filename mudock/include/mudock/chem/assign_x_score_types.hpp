 #pragma once

 #include <functional>
 #include <mudock/chem/x_score_layer.hpp>

 namespace mudock {

  // generic template for x_score layers
  template<typename layer_type>
    requires is_x_score_layer<layer_type>
  void assign_x_score_types(layer_type&);

  // Ligand Specialization (static)
  template<>
  void assign_x_score_types(x_score_static_layer&);

  // Protein specialization (dynamic)
  template<>
  void assign_x_score_types(x_score_dynamic_layer&);

} // namespace mudock