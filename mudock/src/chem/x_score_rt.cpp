#include <algorithm>
#include <mudock/chem/bond_types.hpp>
#include <mudock/chem/x_score_rt.hpp>
#include <mudock/chem/x_score_validity.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <vector>

namespace mudock {

  namespace {

    // Hybridization mark used by XScore's Get_Atom_Hybridizing_Type (molecule.cpp):
    // sp = 1, sp2 = 2, sp3 = 3, none = 4. The classification is based on the basic SYBYL
    // type (mol.atom_type), not the refined X-Tool type.
    int hybridizing_mark(const xtool_ff t) {
      switch (t) {
        case xtool_ff::C3:
        case xtool_ff::N4:
        case xtool_ff::N3:
        case xtool_ff::O3:
        case xtool_ff::P3:
        case xtool_ff::S3:
        case xtool_ff::So:
        case xtool_ff::So2:
        case xtool_ff::Si: return 3; // sp3
        case xtool_ff::C2:
        case xtool_ff::Ccat:
        case xtool_ff::Car:
        case xtool_ff::N2:
        case xtool_ff::Nar:
        case xtool_ff::Npl3:
        case xtool_ff::Nam:
        case xtool_ff::O2:
        case xtool_ff::Oco2:
        case xtool_ff::S2: return 2; // sp2
        case xtool_ff::C1:
        case xtool_ff::N1: return 1; // sp
        default: return 4;           // H, halogens, everything else
      }
    }

    bool is_hydrogen(const xtool_ff t) { return t == xtool_ff::H || t == xtool_ff::Hhb || t == xtool_ff::Hg; }

  } // namespace

  fp_type compute_x_score_rt(const x_score_ligand& xs_lig) {
    const auto& mol     = xs_lig.get_base_molecule();
    const int num_atoms = static_cast<int>(mol.num_atoms());
    const auto bonds    = mol.get_bonds();
    const int num_bonds = static_cast<int>(bonds.size());

    if (num_atoms == 0 || num_bonds == 0)
      return fp_type{0};

    // Basic SYBYL type per atom (used for hybridization / hydrogen detection).
    const auto basic_type = mol.get_atom_type();

    // Adjacency: per-atom incident bonds and heavy-neighbor counts
    std::vector<std::vector<int>> atom_bonds(num_atoms); // incident bond indices per atom
    std::vector<int> num_nonh(num_atoms, 0);             // heavy (non-H) neighbor count per atom
    for (int b = 0; b < num_bonds; ++b) {
      const int a1 = bonds[b].source;
      const int a2 = bonds[b].dest;
      atom_bonds[a1].push_back(b);
      atom_bonds[a2].push_back(b);
      if (!is_hydrogen(basic_type[a2]))
        ++num_nonh[a1];
      if (!is_hydrogen(basic_type[a1]))
        ++num_nonh[a2];
    }

    // Ring-bond detection: a bond is in a ring if it is not a bridge, i.e. its endpoints
    // remain connected after the bond is removed (XScore's Detect_Rings)
    std::vector<char> bond_in_ring(num_bonds, 0);
    std::vector<char> visited(num_atoms, 0);
    std::vector<int> stack;
    for (int b = 0; b < num_bonds; ++b) {
      const int start  = bonds[b].source;
      const int target = bonds[b].dest;
      std::fill(visited.begin(), visited.end(), 0);
      stack.clear();
      stack.push_back(start);
      visited[start] = 1;
      bool reached   = false;
      while (!stack.empty() && !reached) {
        const int cur = stack.back();
        stack.pop_back();
        for (const int nb: atom_bonds[cur]) {
          if (nb == b)
            continue; // the bond under test is removed
          const int other = (bonds[nb].source == cur) ? bonds[nb].dest : bonds[nb].source;
          if (other == target) {
            reached = true;
            break;
          }
          if (!visited[other]) {
            visited[other] = 1;
            stack.push_back(other);
          }
        }
      }
      bond_in_ring[b] = reached ? 1 : 0;
    }

    // Bond validity classification (XScore: 1 = normal, 2 = rotor candidate)
    std::vector<int> bond_valid(num_bonds, 1);
    for (int b = 0; b < num_bonds; ++b) {
      if (bond_in_ring[b])
        continue; // ring bonds are never rotors
      if (bonds[b].type != bond_type::SINGLE)
        continue; // only single bonds can be rotors
      bond_valid[b] = 2;
    }

    // helper: is the heavy atom 'atm' a symmetric terminal hub w.r.t. 'partner'?
    // (XScore Judge_Terminal_Atom: sp3 center, exactly 4 heavy neighbors, the three non-partner
    // ones are terminal heavy atoms carrying the same X-Tool type, e.g. -CMe3, -CF3, -PO3, -NMe3).
    const auto judge_terminal = [&](const int atm, const int partner) -> bool {
      if (hybridizing_mark(basic_type[atm]) != 3)
        return false;
      if (num_nonh[atm] != 4)
        return false;
      int reference = -1; // first non-partner heavy neighbor, to compare X-Tool types against
      for (const int b: atom_bonds[atm]) {
        const int nb = (bonds[b].source == atm) ? bonds[b].dest : bonds[b].source;
        if (is_hydrogen(basic_type[nb]))
          continue;
        if (nb == partner)
          continue;
        if (num_nonh[nb] != 1)
          return false;
        if (reference < 0)
          reference = nb;
        else if (xs_lig.x_score_xtool_type(nb) != xs_lig.x_score_xtool_type(reference))
          return false;
      }
      return true;
    };

    // Eliminate non-rotors among the candidates
    for (int b = 0; b < num_bonds; ++b) {
      if (bond_valid[b] != 2)
        continue;
      const int a1 = bonds[b].source;
      const int a2 = bonds[b].dest;

      // R-H, R-X, R-OH, R-NH2, R-CH3: a bonded heavy atom is terminal (single heavy neighbor)
      if (num_nonh[a1] == 1 || num_nonh[a2] == 1) {
        bond_valid[b] = 1;
        continue;
      }

      // sp2-sp2 (and sp) rotors: both ends sp/sp2
      const int m1 = hybridizing_mark(basic_type[a1]);
      const int m2 = hybridizing_mark(basic_type[a2]);
      int sp_mark  = 0;
      if (m1 == 1 || m1 == 2)
        ++sp_mark;
      if (m2 == 1 || m2 == 2)
        ++sp_mark;
      if (sp_mark == 2) {
        bond_valid[b] = 1;
        continue;
      }

      // terminal symmetric rotors (-CF3, -CMe3, -PO3, -NMe3, ...)
      if (judge_terminal(a1, a2) || judge_terminal(a2, a1)) {
        bond_valid[b] = 1;
        continue;
      }

      // abnormal rotors: an endpoint that failed typing
      if (xs_lig.valid(a1) == x_score_validity::invalid || xs_lig.valid(a2) == x_score_validity::invalid) {
        bond_valid[b] = 1;
        continue;
      }
    }

    // Count the frozen rotors, distributing a per-atom penalty
    fp_type sum = fp_type{0};
    for (int i = 0; i < num_atoms; ++i) {
      if (xs_lig.valid(i) == x_score_validity::invalid)
        continue;

      int mark = 0;
      for (const int b: atom_bonds[i])
        if (bond_valid[b] == 2)
          ++mark;

      if (mark == 1)
        sum += fp_type{0.5};
      else if (mark == 2)
        sum += fp_type{1.0};
      else if (mark >= 3)
        sum += fp_type{0.5};
    }

    return sum;
  }

} // namespace mudock
