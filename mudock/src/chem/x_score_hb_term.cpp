#include <algorithm>
#include <cmath>
#include <mudock/chem/x_score_hb_term.hpp>
#include <mudock/chem/x_score_validity.hpp>
#include <mudock/chem/x_score_xtool_types.hpp>
#include <string_view>
#include <vector>

namespace mudock {

  namespace {

    // small geometry helpers (degrees), mirroring misc.cpp
    fp_type angle_of_two_vectors(const fp_type v1[3], const fp_type v2[3]) {
      const double l1 = std::sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]);
      const double l2 = std::sqrt(v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2]);
      const double dot = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
      double c         = dot / (l1 * l2);
      if (c > 1.0)
        c = 1.0; 
      else if (c < -1.0)
        c = -1.0;
      return static_cast<fp_type>(std::acos(c) / M_PI * 180.0);
    }

    // angle a-b-c (vertex b), in degrees
    fp_type angle_abc(const fp_type a[3], const fp_type b[3], const fp_type c[3]) {
      const fp_type v1[3] = {b[0] - a[0], b[1] - a[1], b[2] - a[2]};
      const fp_type v2[3] = {b[0] - c[0], b[1] - c[1], b[2] - c[2]};
      return angle_of_two_vectors(v1, v2);
    }

    // used in Value_HBond_2
    fp_type ramp(const fp_type v, const fp_type a1, const fp_type a2, const fp_type a3, const fp_type a4) {
      if (v < a1)
        return fp_type{0};
      if (v < a2)
        return (v - a1) / (a2 - a1);
      if (v < a3)
        return fp_type{1};
      if (v < a4)
        return (a4 - v) / (a4 - a3);
      return fp_type{0};
    }

    bool is_hydrogen(const xtool_ff t) {
      return t == xtool_ff::H || t == xtool_ff::Hhb || t == xtool_ff::Hg;
    }

    // X-Tool N.* 
    bool is_N4(const xtool_ff t) { return t == xtool_ff::N4; }
    bool is_N3(const xtool_ff t) { return t == xtool_ff::N3 || t == xtool_ff::N3h || t == xtool_ff::N3un; }
    bool is_Npl3(const xtool_ff t) {
      return t == xtool_ff::Npl3 || t == xtool_ff::Npl3h || t == xtool_ff::Npl3un;
    }
    bool is_N2(const xtool_ff t) { return t == xtool_ff::N2 || t == xtool_ff::N2h || t == xtool_ff::N2un; }
    bool is_Nar(const xtool_ff t) { return t == xtool_ff::Nar || t == xtool_ff::Narh || t == xtool_ff::Narun; }

    char element_of(const xtool_ff basic) {
      switch (basic) {
        case xtool_ff::O3:
        case xtool_ff::O3h:
        case xtool_ff::O3un:
        case xtool_ff::O2:
        case xtool_ff::O2un:
        case xtool_ff::Oco2:
        case xtool_ff::Ow: return 'O';
        case xtool_ff::N4:
        case xtool_ff::N3:
        case xtool_ff::N3h:
        case xtool_ff::N3un:
        case xtool_ff::Npl3:
        case xtool_ff::Npl3h:
        case xtool_ff::Npl3un:
        case xtool_ff::N2:
        case xtool_ff::N2h:
        case xtool_ff::N2un:
        case xtool_ff::Nar:
        case xtool_ff::Narh:
        case xtool_ff::Narun:
        case xtool_ff::N1:
        case xtool_ff::N1un:
        case xtool_ff::Nam: return 'N';
        case xtool_ff::S3:
        case xtool_ff::S3h:
        case xtool_ff::S3un:
        case xtool_ff::S2:
        case xtool_ff::S2un:
        case xtool_ff::So:
        case xtool_ff::So2: return 'S';
        default: return 'X';
      }
    }

    
    int get_donor_type(const int origin,
                       const xtool_ff basic,
                       const xtool_ff xtype,
                       const int num_nonh,
                       const x_score_hb hb,
                       const std::string_view name,
                       const std::string_view residue) {
      if (hb != x_score_hb::D && hb != x_score_hb::DA && hb != x_score_hb::M)
        return 0;

      if (origin == 2) { // protein
        if (hb == x_score_hb::M)
          return 1;
        if (basic == xtool_ff::O3 || basic == xtool_ff::Oco2 || basic == xtool_ff::Ow)
          return 2;
        if (is_N4(xtype) || is_N3(xtype))
          return 2;
        if (is_Npl3(xtype) || is_N2(xtype) || is_Nar(xtype)) {
          if (name.find("NH") != std::string_view::npos && residue.find("ARG") != std::string_view::npos)
            return 2;
          if (name.find("ND") != std::string_view::npos && residue.find("ASN") != std::string_view::npos)
            return 2;
          if (name.find("NE") != std::string_view::npos && residue.find("GLN") != std::string_view::npos)
            return 2;
          return 1;
        }
        if (basic == xtool_ff::S3)
          return 2;
        return 2;
      }

      // ligand
      if (basic == xtool_ff::O3 || basic == xtool_ff::Oco2)
        return 2;
      if (is_N4(xtype) || is_N3(xtype))
        return (num_nonh <= 2) ? 2 : 1;
      if (is_Npl3(xtype) || is_N2(xtype) || is_Nar(xtype))
        return (num_nonh <= 1) ? 2 : 1;
      if (basic == xtool_ff::S3)
        return 2;
      return 2;
    }

    
    int get_acceptor_type(const int origin,
                          const xtool_ff basic,
                          const xtool_ff xtype,
                          const int num_nonh,
                          const x_score_hb hb) {
      if (hb != x_score_hb::A && hb != x_score_hb::DA)
        return 0;

      if (origin == 2) { // protein
        if (basic == xtool_ff::O3 || basic == xtool_ff::O2 || basic == xtool_ff::Oco2 ||
            basic == xtool_ff::Ow)
          return 2;
        if (is_Npl3(xtype) || is_N2(xtype) || is_Nar(xtype))
          return 1;
        return 2;
      }

      // ligand
      if (basic == xtool_ff::O3 || basic == xtool_ff::O2 || basic == xtool_ff::Oco2)
        return 2;
      if (is_Npl3(xtype) || is_N2(xtype) || is_Nar(xtype))
        return (num_nonh <= 1) ? 2 : 1;
      return 2;
    }

    
    template<typename layer_t>
    std::vector<std::vector<int>> build_adjacency(const layer_t& layer, const int num_atoms) {
      const auto& mol  = layer.get_base_molecule();
      const auto bonds = mol.get_bonds();
      std::vector<std::vector<int>> adj(num_atoms);
      for (const auto& b: bonds) {
        adj[b.source].push_back(b.dest);
        adj[b.dest].push_back(b.source);
      }
      return adj;
    }

    
    bool hb_eligible(const x_score_hb hb, const bool is_protein) {
      if (hb == x_score_hb::D || hb == x_score_hb::A || hb == x_score_hb::DA)
        return true;
      return is_protein && hb == x_score_hb::M;
    }

    template<typename layer_t>
    std::vector<x_score_hb_atom> build_hb_atoms(const layer_t& layer, const bool is_protein) {
      const auto& mol     = layer.get_base_molecule();
      const int num_atoms = static_cast<int>(mol.num_atoms());
      const int origin    = is_protein ? 2 : 1;
      const auto adj      = build_adjacency(layer, num_atoms);

      std::vector<x_score_hb_atom> atoms;
      atoms.reserve(num_atoms);

      // filtering of atoms that are not useful for Hydrogen Bonding
      for (int i = 0; i < num_atoms; ++i) {
        if (layer.valid(i) == x_score_validity::invalid)
          continue;
        const xtool_ff basic = mol.atom_type(i);
        if (is_hydrogen(basic))
          continue;
        if (basic == xtool_ff::Ow)
          continue; // water excluded from the protein HB list (and ligands have none)
        const x_score_hb hb = layer.hb(i);
        if (!hb_eligible(hb, is_protein))
          continue;

        // heavy-neighbour root + neighbour counts
        fp_type rx = 0, ry = 0, rz = 0;
        int num_nonh = 0, num_h = 0;
        for (const int nb: adj[i]) {
          if (is_hydrogen(mol.atom_type(nb))) {
            ++num_h;
          } else if (layer.hb(nb) == x_score_hb::M) {
            // Exclude metal neighbours to match XScore.
            continue;
          } else {
            rx += mol.x(nb);
            ry += mol.y(nb);
            rz += mol.z(nb);
            ++num_nonh;
          }
        }

        x_score_hb_atom a;
        a.x        = mol.x(i);
        a.y        = mol.y(i);
        a.z        = mol.z(i);
        a.radius   = layer.vdw_radius(i);
        a.hb       = static_cast<int>(hb);
        a.is_metal = (hb == x_score_hb::M);
        a.is_water = false;
        a.num_h    = num_h;
        a.element  = element_of(basic);
        if (num_nonh > 0) {
          a.rx       = rx / num_nonh;
          a.ry       = ry / num_nonh;
          a.rz       = rz / num_nonh;
          a.has_root = true;
        } else {
          a.rx = a.x;
          a.ry = a.y;
          a.rz = a.z;
          a.has_root = false;
        }

        const xtool_ff xtype = layer.x_score_xtool_type(i);
        std::string_view name, residue;
        if (is_protein) {
          name = mol.atom_name(i);

          const int rid = static_cast<int>(mol.residue_types(i));
          if (rid >= 0 && rid < num_residues())
            residue = get_description(mol.residue_types(i)).name;
        }
        a.donor_type    = get_donor_type(origin, basic, xtype, num_nonh, hb, name, residue);
        a.acceptor_type = get_acceptor_type(origin, basic, xtype, num_nonh, hb);

        atoms.push_back(a);
      }

      return atoms;
    }


    fp_type value_hbond_2(const x_score_hb_atom& D, const x_score_hb_atom& A) {
      const fp_type dco[3] = {D.x, D.y, D.z};
      const fp_type aco[3] = {A.x, A.y, A.z};
      const fp_type dro[3] = {D.rx, D.ry, D.rz};
      const fp_type aro[3] = {A.rx, A.ry, A.rz};

      const fp_type d = std::sqrt((D.x - A.x) * (D.x - A.x) + (D.y - A.y) * (D.y - A.y) +
                                  (D.z - A.z) * (D.z - A.z));

      // DR-D-A angle
      bool mark1   = false;
      fp_type a1   = 0;
      if (!(D.is_metal || D.is_water) && D.has_root) {
        a1    = fp_type{180} - angle_abc(dro, dco, aco);
        mark1 = true;
      }

      // D-A-AR angle
      bool mark2 = false;
      fp_type a2 = 0;
      if (!(A.is_metal || A.is_water) && A.has_root) {
        a2    = fp_type{180} - angle_abc(dco, aco, aro);
        mark2 = true;
      }

      const int d_type = D.donor_type;
      const int a_type = A.acceptor_type;

      const fp_type d0 = D.radius + A.radius;
      const fp_type d1 = 0, d2 = 1, d3 = d0 - fp_type{0.4}, d4 = d0 + fp_type{0.2};

      fp_type tmp1;
      if (d < d1)
        tmp1 = 0;
      else if (d < d2)
        tmp1 = (d - d1) / (d2 - d1);
      else if (d < d3)
        tmp1 = 1;
      else if (d < d4)
        tmp1 = (d4 - d) / (d4 - d3);
      else
        tmp1 = 0;

      fp_type tmp3 = 1;
      if (mark1) {
        if (d_type == 1)
          tmp3 = ramp(a1, 0, fp_type{0.001}, 25, 50);
        else
          tmp3 = ramp(a1, 25, 50, 75, 100);
      }

      fp_type tmp4 = 1;
      if (mark2) {
        if (a_type == 1)
          tmp4 = ramp(a2, 0, fp_type{0.001}, 30, 55);
        else
          tmp4 = ramp(a2, 0, 5, 70, 95);
      }

      return (tmp3 >= tmp4) ? tmp1 * tmp4 : tmp1 * tmp3;
    }

  } // namespace

  std::vector<x_score_hb_atom> build_protein_hb_atoms(const x_score_protein& prot) {
    return build_hb_atoms(prot, /*is_protein=*/true);
  }

  std::vector<x_score_hb_atom> build_ligand_hb_atoms(const x_score_ligand& lig) {
    return build_hb_atoms(lig, /*is_protein=*/false);
  }

  fp_type compute_x_score_hb(const std::vector<x_score_hb_atom>& lig_atoms,
                             const std::vector<x_score_hb_atom>& prot_atoms) {
    constexpr fp_type cutoff = fp_type{5.0};

    // One candidate H-bond between a ligand atom and a protein atom.
    struct candidate {
      int li;       // index into lig_atoms (== latom group key)
      int pj;       // index into prot_atoms
      int type;     // 1: lig donor; 2: lig acceptor / prot donor; 3: lig acceptor / metal
      fp_type score;
    };
    std::vector<candidate> cand;

    // Get_HBond_Pair_PL: generate scored candidates
    for (int li = 0; li < static_cast<int>(lig_atoms.size()); ++li) {
      const auto& L = lig_atoms[li];
      const auto lhb = static_cast<x_score_hb>(L.hb);

      for (int pj = 0; pj < static_cast<int>(prot_atoms.size()); ++pj) {
        const auto& P  = prot_atoms[pj];
        const auto phb = static_cast<x_score_hb>(P.hb);

        // determine the H-bond type from the donor/acceptor characters
        int type = 0;
        if (lhb == x_score_hb::D) {
          if (phb == x_score_hb::A || phb == x_score_hb::DA)
            type = 1;
        } else if (lhb == x_score_hb::A) {
          if (phb == x_score_hb::D || phb == x_score_hb::DA)
            type = 2;
          else if (phb == x_score_hb::M)
            type = 3;
        } else if (lhb == x_score_hb::DA) {
          if (phb == x_score_hb::A || phb == x_score_hb::DA)
            type = 1;
          else if (phb == x_score_hb::D)
            type = 2;
          else if (phb == x_score_hb::M)
            type = 3;
        }
        if (type == 0)
          continue;

        const fp_type d = std::sqrt((L.x - P.x) * (L.x - P.x) + (L.y - P.y) * (L.y - P.y) +
                                    (L.z - P.z) * (L.z - P.z));
        if (d > cutoff)
          continue;

        // donor/acceptor assignment then geometric strength
        fp_type score;
        if (type == 1)
          score = value_hbond_2(L, P); // ligand donor, protein acceptor
        else
          score = value_hbond_2(P, L); // protein donor (or metal), ligand acceptor

        if (std::fabs(score) > fp_type{0})
          cand.push_back({li, pj, type, score});
      }
    }

    const int n = static_cast<int>(cand.size());

    // Sum_HBonds: rank and filter
    // Step 1: decreasing |score|
    std::stable_sort(cand.begin(), cand.end(), [](const candidate& a, const candidate& b) {
      return std::fabs(a.score) > std::fabs(b.score);
    });

    // Step 2: two H-bonds on the same ligand atom must be at least 45 degrees apart
    for (int i = 0; i < n - 1; ++i)
      for (int j = i + 1; j < n; ++j) {
        if (cand[i].li != cand[j].li)
          continue;
        const auto& Li = lig_atoms[cand[i].li];
        const auto& Pi = prot_atoms[cand[i].pj];
        const auto& Lj = lig_atoms[cand[j].li];
        const auto& Pj = prot_atoms[cand[j].pj];
        const fp_type v1[3] = {Pi.x - Li.x, Pi.y - Li.y, Pi.z - Li.z};
        const fp_type v2[3] = {Pj.x - Lj.x, Pj.y - Lj.y, Pj.z - Lj.z};
        const fp_type angle = std::fabs(angle_of_two_vectors(v1, v2));
        if (angle < fp_type{45})
          cand[j].score = 0;
      }

    // Step 3: a donor ligand atom forms no more H-bonds than the hydrogens it carries
    for (int i = 0; i < n - 1; ++i) {
      if (cand[i].type != 1)
        continue;
      int count          = 1;
      const auto& L      = lig_atoms[cand[i].li];
      const int limit    = (static_cast<x_score_hb>(L.hb) == x_score_hb::DA) ? 1 : L.num_h;
      for (int j = i + 1; j < n; ++j) {
        if (cand[j].type != 1 || cand[i].li != cand[j].li)
          continue;
        ++count;
        if (count > limit)
          cand[j].score = 0;
      }
    }

    // Step 4: an acceptor ligand atom forms no more H-bonds than its lone pairs
    for (int i = 0; i < n - 1; ++i) {
      if (cand[i].type != 2 && cand[i].type != 3)
        continue;
      int count     = 1;
      const auto& L = lig_atoms[cand[i].li];
      int limit     = 2;
      if (L.element == 'O')
        limit = 2;
      else if (L.element == 'N')
        limit = 1;
      else if (L.element == 'S')
        limit = 2;
      for (int j = i + 1; j < n; ++j) {
        if ((cand[j].type != 2 && cand[j].type != 3) || cand[i].li != cand[j].li)
          continue;
        ++count;
        if (count > limit)
          cand[j].score = 0;
      }
    }

    // sum the surviving contributions
    fp_type sum = 0;
    for (const auto& c: cand)
      if (std::fabs(c.score) >= fp_type{0.01})
        sum += c.score;

    return sum;
  }

} // namespace mudock