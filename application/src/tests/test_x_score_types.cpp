#include <iostream>
    #include <string>
    #include <mudock/format/reader.hpp>
    #include <mudock/chem/x_score_ligand.hpp>
    #include <mudock/chem/elements.hpp>
    #include <mudock/chem/x_score_xtool_types.hpp>

    void test_x_score_typing(const std::string& ligand_mol2_path) {
        // 1. Parsing
        mudock::static_molecule ligand = mudock::parser<mudock::static_molecule>(ligand_mol2_path);

	// 2. Tipizzazione topologica
        mudock::x_score_ligand xs_lig{ligand};

        const std::size_t num_atoms = ligand.num_atoms();
        const auto atom_names = ligand.get_atom_name();
        const auto atom_types = ligand.get_atom_type();

	// 3. Stampa nel formato richiesto
        for (std::size_t i = 0; i < num_atoms; ++i) {
            std::string name = atom_names[i];
            auto xtool_type = xs_lig.x_score_xtool_type(i);
            auto atom_type = atom_types[i];

            // Estrae il nome in formato testuale dall'enum (es. "C.2.x")
            auto type_string_name = mudock::get_description(xtool_type).name;
            auto atom_type_string = mudock::get_description(atom_type).name;
            auto vdw_r = xs_lig.vdw_radius(i);

            // Print formattato
            std::cout << "Atom Name: " << name
                      << " atom type: " << atom_type_string
                      << " xtool type:          " << type_string_name
                      << " vdw radius: " << vdw_r << "\n";
	}
    }

    int main(int argc, char** argv) {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <path_to_ligand.mol2>\n";
            return 1;
        }

        std::string ligand_path = argv[1];

        try {
            test_x_score_typing(ligand_path);
        } catch (const std::exception& e) {
            std::cerr << "Error during execution: " << e.what() << "\n";
            return 1;
        }

        return 0;
    }