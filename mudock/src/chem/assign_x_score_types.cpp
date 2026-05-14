#include <mudock/chem/assign_autodock_babel_types.hpp>
#include <mudock/chem/assign_x_score_types.hpp>
#include <mudock/chem/x_score_xlogp_types.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/chem/x_score_residue_xtool_types.hpp>
#include <mudock/chem/autodock_babel_types.hpp>
#include <mudock/chem/elements.hpp>
#include <mudock/molecule.hpp>
#include <mudock/molecule/graph.hpp>
#include <span>

namespace mudock {

  template<class molecule_type>
    requires is_molecule<molecule_type>
  void assign_x_score_types_impl(molecule_type& mol) {

    // differentiate logic between xlogp and xtool types


    // todo: likely to be removed
    // assign the autodock babel type
/*     assign_autodock_babel_types(make_span(babel_types, num_atoms),
                                mol.get_x(),
                                mol.get_y(),
                                mol.get_z(),
                                mol.get_elements(),
                                graph); */


  }

  template<>
  void assign_x_score_types(static_molecule& mol, std::function<void(static_molecule&)> f) {
    if (f)
      f(mol);
    else
      assign_x_score_types_impl(mol);
  };
  template<>
  void assign_x_score_types(dynamic_molecule& mol, std::function<void(dynamic_molecule&)> f) {
    if (f)
      f(mol);
    else
      assign_x_score_types_impl(mol);
  };

} // namespace mudock
