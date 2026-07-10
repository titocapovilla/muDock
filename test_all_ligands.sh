#!/usr/bin/env bash
#
# Full cross-product scoring: for EVERY protein PDB, score EVERY ligand .mol2 against it with muDock's
# X-Score (x_score_bench). One bench invocation per (protein, ligand) pair. The side-by-side layout of
# test.sh is mirrored, but here each protein gets a block listing all ligands scored against it. Results
# are written to output_all_ligands.txt.
#
# NOTE: ligands keep the crystal coordinates of their own complex, so only the *native* protein-ligand
# pair sits in the pocket. Off-diagonal pairs (ligand X vs protein Y, X != Y) are far from Y's pocket, so
# their pairwise terms (VDW, HB, HP) come out ~0; RT is ligand-intrinsic and stays non-zero. That is the
# expected physics without re-docking -- this script is a coverage/robustness sweep of the scoring path,
# not a re-docking experiment.
#
# Usage:  ./test_all_ligands.sh
#
set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# --- locations (edit here if your layout differs) ---
PROT_DIR="$HOME/xscore-dev/srcFiles/proteins/mudock_proteins"
LIG_DIR="$HOME/xscore-dev/srcFiles/ligands/mudock_ligands"
BENCH="$ROOT/build/application/bench/x_score_bench"

OUT="$ROOT/output_all_ligands.txt"

# Extract the implemented terms (VDW, HB, HP, RT) from the bench output. The bench prints one
# "Ligand <name> terms  VDW=<> HB=<> HP=<> RT=<>" line per ligand; with a single-ligand file there is
# exactly one, so pulling the first match of each term is unambiguous.
terms_of() {
  # $1 = raw multi-line bench output ; echoes "VDW=<> HB=<> HP=<> RT=<>"
  local text="$1" vdw hb hp rt
  vdw="$(printf '%s\n' "$text" | grep -oE 'VDW=[^ ]+' | head -1)"
  hb="$(printf '%s\n' "$text"  | grep -oE 'HB=[^ ]+'  | head -1)"
  hp="$(printf '%s\n' "$text"  | grep -oE 'HP=[^ ]+'  | head -1)"
  rt="$(printf '%s\n' "$text"  | grep -oE 'RT=[^ ]+'  | head -1)"
  echo "${vdw:-VDW=?} ${hb:-HB=?} ${hp:-HP=?} ${rt:-RT=?}"
}

# --- sanity checks ---
if [ ! -x "$BENCH" ]; then
  echo "x_score_bench not found at $BENCH — building it ..."
  cmake --build "$ROOT/build" --target x_score_bench -j4 || { echo "build failed"; exit 1; }
fi
[ -d "$PROT_DIR" ] || { echo "protein dir not found: $PROT_DIR"; exit 1; }
[ -d "$LIG_DIR" ]  || { echo "ligand dir not found: $LIG_DIR"; exit 1; }

num_prot=$(ls "$PROT_DIR"/*_protein.pdb 2>/dev/null | wc -l)
num_lig=$(ls "$LIG_DIR"/*_ligand.mol2 2>/dev/null | wc -l)
echo "Scoring $num_lig ligand(s) against each of $num_prot protein(s)  (= $((num_prot * num_lig)) pairs)."

: > "$OUT"

pcount=0
for pdb in "$PROT_DIR"/*_protein.pdb; do
  [ -e "$pdb" ] || continue
  pname="$(basename "$pdb" _protein.pdb)"
  pcount=$((pcount + 1))
  echo "[$pcount/$num_prot] protein $pname ..."

  {
    echo "=== protein $pname ==="
  } >> "$OUT"

  for lig in "$LIG_DIR"/*_ligand.mol2; do
    [ -e "$lig" ] || continue
    lname="$(basename "$lig" _ligand.mol2)"
    md="$(terms_of "$("$BENCH" -p "$pdb" -l "$lig" 2>&1)")"
    printf '    %-10s %s\n' "$lname" "${md:-<no output>}" >> "$OUT"
  done

  echo >> "$OUT"
done

echo "Done: $((pcount * num_lig)) pair(s) written to $OUT"
