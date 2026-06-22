#include <iostream>
#include <string>
#include <iomanip>
#include <mudock/format/reader.hpp>
#include <mudock/chem/x_score_ligand.hpp>
#include <mudock/chem/x_score_protein.hpp>
#include <mudock/chem/elements.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/chem/x_score_residue_xtool_types.hpp>

void test_x_score_typing_ligand(const std::string& ligand_mol2_path) {
    std::cout << "Testing ligand: " << ligand_mol2_path << "\n";
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
        std::cout << std::left 
                  << " atom type: " << std::setw(10) << atom_type_string
                  << " xtool type: " << std::setw(15) << type_string_name
                  << " vdw radius: " << vdw_r << "\n";
    }
}

void test_x_score_typing_protein(const std::string& protein_path) {
    std::cout << "Testing protein: " << protein_path << "\n";
    // 1. Parsing
    mudock::dynamic_molecule protein = mudock::parser<mudock::dynamic_molecule>(protein_path);

    // 2. Tipizzazione topologica
    mudock::x_score_protein xs_prot{protein};

    const std::size_t num_atoms = protein.num_atoms();
    const auto atom_names = protein.get_atom_name();

    // 3. Stampa nel formato richiesto
    for (std::size_t i = 0; i < num_atoms; ++i) {
        std::string name = atom_names[i];
        auto residue = protein.residue_types(i);
        auto xtool_type = xs_prot.x_score_xtool_type(i);
        
        auto type_string_name = mudock::get_description(xtool_type).name;
        auto vdw_r = xs_prot.vdw_radius(i);

        // Print formattato
        std::cout << std::left 
                  << " atom name: " << std::setw(10) << name
                  << " residue: " << std::setw(10) << mudock::get_description(residue).name
                  << " xtool type: " << std::setw(15) << type_string_name
                  << " vdw radius: " << vdw_r << "\n";
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <ligand|protein> <path_to_file>\n";
        return 1;
    }

    std::string type = argv[1];
    std::string path = argv[2];

    try {
        if (type == "ligand") {
            test_x_score_typing_ligand(path);
        } else if (type == "protein") {
            test_x_score_typing_protein(path);
        } else {
            std::cerr << "Unknown type: " << type << ". Please use 'ligand' or 'protein'.\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << "\n";
        return 1;
    }

    return 0;
}