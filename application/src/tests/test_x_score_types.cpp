#include <cmath>
#include <iomanip>
#include <iostream>
#include <mudock/chem/elements.hpp>
#include <mudock/chem/x_score_ligand.hpp>
#include <mudock/chem/x_score_protein.hpp>
#include <mudock/chem/x_score_residue_xtool_types.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <mudock/format/reader.hpp>
#include <string>

#define DIST_CUTOFF 8.00

void test_x_score_typing_ligand(const mudock::x_score_ligand& xs_lig) {
  const auto& ligand          = xs_lig.get_base_molecule();
  const std::size_t num_atoms = ligand.num_atoms();
  const auto atom_names       = ligand.get_atom_name();
  const auto atom_types       = ligand.get_atom_type();

  // 3. Stampa nel formato richiesto
  for (std::size_t i = 0; i < num_atoms; ++i) {
    std::string name = atom_names[i];
    auto xtool_type  = xs_lig.x_score_xtool_type(i);
    auto atom_type   = atom_types[i];

    // Estrae il nome in formato testuale dall'enum (es. "C.2.x")
    auto type_string_name = mudock::get_description(xtool_type).name;
    auto atom_type_string = mudock::get_description(atom_type).name;
    auto vdw_r            = xs_lig.vdw_radius(i);

    // Print formattato
    std::cout << std::left << " atom type: " << std::setw(10) << atom_type_string
              << " xtool type: " << std::setw(15) << type_string_name << " vdw radius: " << vdw_r << "\n";
  }
}

void test_x_score_typing_protein(const mudock::x_score_protein& xs_prot) {
  const auto& protein         = xs_prot.get_base_molecule();
  const std::size_t num_atoms = protein.num_atoms();
  const auto atom_names       = protein.get_atom_name();

  // 3. Stampa nel formato richiesto
  for (std::size_t i = 0; i < num_atoms; ++i) {
    std::string name = atom_names[i];
    auto residue     = protein.residue_types(i);
    auto xtool_type  = xs_prot.x_score_xtool_type(i);

    auto type_string_name = mudock::get_description(xtool_type).name;
    auto vdw_r            = xs_prot.vdw_radius(i);

    // Print formattato
    std::cout << std::left << " atom name: " << std::setw(10) << name << " residue: " << std::setw(10)
              << mudock::get_description(residue).name << " xtool type: " << std::setw(15) << type_string_name
              << " vdw radius: " << vdw_r << "\n";
  }
}

float calculate_distance(float x1, float y1, float z1, float x2, float y2, float z2) {
  float dx = x1 - x2;
  float dy = y1 - y2;
  float dz = z1 - z2;
  return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float calculate_vdw(const mudock::x_score_ligand& xs_lig, const mudock::x_score_protein& xs_prot) {
  const auto& ligand  = xs_lig.get_base_molecule();
  const auto& protein = xs_prot.get_base_molecule();
  float asum          = 0.0f;
  float sum           = 0.0f;

  const std::size_t num_ligand_atoms = ligand.num_atoms();

  //cycle all ligand atoms
  for (std::size_t i = 0; i < num_ligand_atoms; ++i) {
    // todo: check if atom is valid
    //if (ligand.atom_type(i) == mudock::xtool_ff::H) continue;
    //if (ligand.atom_type(i) == mudock::xtool_ff::Hhb) continue;
    //if (ligand.atom_type(i) == mudock::xtool_ff::Hg) continue;

    asum                                = 0.000;
    const std::size_t num_protein_atoms = protein.num_atoms();

    for (std::size_t j = 0; j < num_protein_atoms; ++j) {
      //todo check if atom is valid
      //if (protein.atom_type(j) == mudock::xtool_ff::H) continue;
      //if (protein.atom_type(j) == mudock::xtool_ff::Hhb) continue;
      //if (protein.atom_type(j) == mudock::xtool_ff::Hg) continue;
      //if (protein.atom_type(j) == mudock::xtool_ff::Ow) continue;

      float d0  = xs_lig.vdw_radius(i) + xs_prot.vdw_radius(j);
      float l_x = ligand.x(i);
      float l_y = ligand.y(i);
      float l_z = ligand.z(i);
      float p_x = protein.x(j);
      float p_y = protein.y(j);
      float p_z = protein.z(j);

      float d =
          calculate_distance(ligand.x(i), ligand.y(i), ligand.z(i), protein.x(j), protein.y(j), protein.z(j));

      int dummy = 0;
      
      std::cout << "protein atom_type: " << mudock::get_description(protein.atom_type(j)).name << "\n";
      std::cout << "d0: " << std::setw(10) << d0 << "\t" << "d: " << std::setw(15) << d << "\n";

      if (d > DIST_CUTOFF)
        continue;

      float tmp1 = d0 / d;                    //  +Tito: some division of radius/positon
      tmp1       = tmp1 * tmp1 * tmp1 * tmp1; // +Tito: distance ^4
      float tmp2 = tmp1 * tmp1;               // +Tito: distance ^2
      float tmp  = tmp2 - 2.00 * tmp1;        // +Tito: total sum

      asum += tmp; //+Tito: update asum
    }

    asum *= (-1.00); // change the sign so that positive values are favorable

    // if this atom has unfavorable contribution then neglect it

    if (asum < 0.0f)
      continue; //+Tito: asum is unfavourable then skip
    else {      // +Tito: else add the contribution to the sum and assign it to the atom's score
      sum += asum;
      //ligand.atom.score=asum;
    }
  }

  return sum;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <ligand|protein|vdw> <path_to_file> [path_to_protein (for vdw)]\n";
    return 1;
  }

  std::string type = argv[1];
  std::string path = argv[2];

  try {
    if (type == "ligand") {
      std::cout << "Testing ligand: " << path << "\n";
      mudock::static_molecule ligand = mudock::parser<mudock::static_molecule>(path);
      mudock::x_score_ligand xs_lig{ligand};
      test_x_score_typing_ligand(xs_lig);
    } else if (type == "protein") {
      std::cout << "Testing protein: " << path << "\n";
      mudock::dynamic_molecule protein = mudock::parser<mudock::dynamic_molecule>(path);
      mudock::x_score_protein xs_prot{protein};
      test_x_score_typing_protein(xs_prot);
    } else if (type == "vdw") {
      if (argc < 4) {
        std::cerr << "Error: 'vdw' requires both a ligand path and a protein path.\n";
        std::cerr << "Usage: " << argv[0] << " vdw <path_to_ligand> <path_to_protein>\n";
        return 1;
      }
      std::string prot_path = argv[3];
      std::cout << "Testing VDW between ligand: " << path << " and protein: " << prot_path << "\n";

      mudock::static_molecule ligand = mudock::parser<mudock::static_molecule>(path);
      mudock::x_score_ligand xs_lig{ligand};

      mudock::dynamic_molecule protein = mudock::parser<mudock::dynamic_molecule>(prot_path);
      mudock::x_score_protein xs_prot{protein};

      // Print ligand information
      //test_x_score_typing_ligand(xs_lig);

      // Print protein information
      //test_x_score_typing_protein(xs_prot);

      // Calculate and print VDW score
      float vdw_score = calculate_vdw(xs_lig, xs_prot);
      std::cout << "\nCalculated VDW Score: " << vdw_score << "\n";
    } else {
      std::cerr << "Unknown type: " << type << ". Please use 'ligand', 'protein', or 'vdw'.\n";
      return 1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error during execution: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
