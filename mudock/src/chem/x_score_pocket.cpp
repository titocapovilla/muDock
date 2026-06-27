#include <cmath>
#include <mudock/chem/x_score_pocket.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace mudock {

  namespace {

    // XScore's Define_Pocket filters on hydrogens via the SYBYL `type` string ("H").
    // Both non-polar (H) and H-bond (Hhb) xtool types map to SYBYL "H".
    inline bool is_hydrogen(const xtool_ff t) { return t == xtool_ff::H || t == xtool_ff::Hhb; }

    inline fp_type distance(const fp_type x1,
                            const fp_type y1,
                            const fp_type z1,
                            const fp_type x2,
                            const fp_type y2,
                            const fp_type z2) {
      const fp_type dx = x1 - x2;
      const fp_type dy = y1 - y2;
      const fp_type dz = z1 - z2;
      return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    // Identifies a residue instance: residue type + sequence id + chain.
    struct residue_key {
      residue res;
      int id;
      char chain;
      bool matches(const residue r, const int i, const char c) const {
        return res == r && id == i && chain == c;
      }
    };

  } // namespace

  void define_pocket(x_score_protein& protein, const x_score_ligand& ligand, const fp_type cutoff) {
    auto& pmol            = protein.get_base_molecule();
    const auto& lmol      = ligand.get_base_molecule();
    const int num_protein = pmol.num_atoms();
    const int num_ligand  = lmol.num_atoms();

    std::vector<residue_key> pocket_res;

    // --- Stage 1: mark pocket atoms within `cutoff` of a valid non-H ligand atom ---
    // (XScore protein.cpp:1354-1412)
    for (int i = 0; i < num_protein; ++i) {
      if (protein.valid(i) == x_score_validity::invalid)
        continue;

      // filter out non-polar hydrogens (X-Score uses a unit-atom model)
      if (protein.x_score_xtool_type(i) == xtool_ff::H) {
        protein.valid(i) = x_score_validity::invalid;
        continue;
      }

      // is this atom close to the ligand?
      bool close = false;
      for (int j = 0; j < num_ligand && !close; ++j) {
        if (ligand.valid(j) == x_score_validity::invalid)
          continue;
        if (is_hydrogen(ligand.x_score_xtool_type(j)))
          continue;
        const fp_type d =
            distance(pmol.x(i), pmol.y(i), pmol.z(i), lmol.x(j), lmol.y(j), lmol.z(j));
        if (d <= cutoff)
          close = true;
      }

      if (!close)
        continue; // not a pocket atom

      protein.valid(i) = x_score_validity::pocket;

      // waters and metal ions are pocket atoms but are not recorded as pocket residues
      const auto xt = protein.x_score_xtool_type(i);
      if (xt == xtool_ff::Ow || xt == xtool_ff::Mplus)
        continue;

      // record a newly found pocket residue (keyed by residue + id + chain)
      const residue r = pmol.residue_types(i);
      const int rid   = pmol.res_id(i);
      const char ch   = pmol.chain(i);
      bool known      = false;
      for (const auto& k: pocket_res) {
        if (k.matches(r, rid, ch)) {
          known = true;
          break;
        }
      }
      if (!known)
        pocket_res.push_back(residue_key{r, rid, ch});
    }

    // --- Guard: the binding pocket must be well defined ---
    // (XScore protein.cpp:1416-1432) count amino-acid residues, excluding hetero/water.
    int amino_acid_res = 0;
    for (const auto& k: pocket_res) {
      if (k.res == residue::HET || k.res == residue::HOH)
        continue;
      ++amino_acid_res;
    }
    if (amino_acid_res < 3) {
      throw std::runtime_error("define_pocket: cannot find binding pocket residues on the protein. "
                               "Probably the ligand has not been docked with the protein.");
    }

    // --- Stage 2: expand the pocket to all atoms of the pocket residues ---
    // (XScore protein.cpp:1436-1450)
    for (int i = 0; i < num_protein; ++i) {
      if (protein.valid(i) == x_score_validity::invalid)
        continue;
      if (protein.valid(i) == x_score_validity::pocket)
        continue;
      const auto xt = protein.x_score_xtool_type(i);
      if (xt == xtool_ff::Ow || xt == xtool_ff::Mplus)
        continue;

      const residue r = pmol.residue_types(i);
      const int rid   = pmol.res_id(i);
      const char ch   = pmol.chain(i);
      for (const auto& k: pocket_res) {
        if (k.matches(r, rid, ch)) {
          protein.valid(i) = x_score_validity::pocket;
          break;
        }
      }
    }
  }

} // namespace mudock
