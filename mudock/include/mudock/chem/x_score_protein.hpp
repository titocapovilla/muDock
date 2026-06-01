#pragma once

#include <mudock/chem/x_score_layer.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/chem/x_score_xlogp_types.hpp>
#include <mudock/chem/x_score_xtool_residue_types.hpp>
#include <mudock/chem/assign_x_score_types.hpp> //modify types with layer types


#include <mudock/molecule.hpp>
#include <mudock/type_alias.hpp>



namespace mudock { 
 struct x_score_protein: public x_score_dynamic_layer {

	x_score_protein(dynamic_molecule& _molecule) : x_score_dynamic_layer(_molecule) {
    // Base class constructor already resized the arrays
    prepare();
	}



	private:

	    void prepare() override {
        // todo implement logic
			  // 1. Assign atom types based on residue dictionaries (protein logic)
				// This will call the dynamic_molecule specialization
				//assign_x_score_types(this->get_base_molecule());

				// 2. Populate physical parameters specifically for the protein
				// Could involve looking up residue-specific properties or standard XTOOL properties
				//for (int i = 0; i < this->num_atoms(); ++i) {
							// ... population logic ...
				//}

				// 3. Grid generation or other protein-specific precomputations
				// (Similar to autodock_protein::prepare)

    }

	};
}