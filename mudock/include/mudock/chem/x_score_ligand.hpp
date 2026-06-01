#pragma once

#include <mudock/chem/x_score_layer.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/chem/x_score_xlogp_types.hpp>
#include <mudock/chem/x_score_xtool_residue_types.hpp>
#include <mudock/chem/assign_x_score_types.hpp> //modify types with layer types

#include <mudock/molecule.hpp>
#include <mudock/type_alias.hpp>



namespace mudock { 
 struct x_score_ligand: public x_score_static_layer {

		x_score_ligand(static_molecule& _molecule) : x_score_static_layer(_molecule) {
			// Base class constructor already resized the arrays
			prepare();
		}


	private:


		void prepare() override {
				// todo implement logic of determining xtool and xlogp type for ligangs

				// 1. Assign atom types based on chemical perception (ligand logic)
				//assign_x_score_types(this->get_base_molecule());

				// 2. Populate physical parameters from the XTOOL dictionary
				//for (int i = 0; i < this->num_atoms(); ++i) {
						// Assuming the typing function placed the correct xtool_ff in the molecule or layer
						// If it's stored in the base molecule, retrieve it; otherwise if stored in the layer directly:
						// const auto type = this->atom_x_score_xtool_type[i];

						// Example of populating layer arrays:
						// const auto& ff_entry = get_description(type);
						// this->vdw_radius[i] = ff_entry.vdw_radius;
						// ... populate other fields ...
				//}
		}
	};
}