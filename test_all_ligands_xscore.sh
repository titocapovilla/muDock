#!/usr/bin/env bash
#
# XScore counterpart of test_all_ligands.sh: for EVERY protein PDB, score EVERY ligand .mol2 against it
# with the reference XScore. One xscore invocation per (protein, ligand) pair. The output is written to
# output_all_ligands_xscore.txt with the *exact same structure* as output_all_ligands.txt so the two can
# be compared directly:
#
#     diff output_all_ligands.txt output_all_ligands_xscore.txt
#
# IMPORTANT -- off-diagonal behaviour differs by design:
#   * muDock has no pocket filter, so a ligand far from the protein scores VDW=HB=HP=0 (RT is intrinsic).
#   * XScore *aborts* such a pair with "cannot find binding pocket residues on the protein" and prints no
#     terms. Here that is recorded as "VDW=? HB=? HP=? RT=?" (the terms_of fallback), keeping the two
#     files line-aligned. Consequently a diff is dominated by off-diagonal rows; the meaningful signal is
#     the ~N native (diagonal) pairs, where the two tools should agree to float precision.
#
# Usage:  ./test_all_ligands_xscore.sh
#
set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# --- locations (edit here if your layout differs) ---
PROT_DIR="$HOME/xscore-dev/srcFiles/proteins/mudock_proteins"
LIG_DIR="$HOME/xscore-dev/srcFiles/ligands/mudock_ligands"
XSCORE="$HOME/xscore-dev/xscore_v1.3/c++/xscore"
export XSCORE_PARAMETER="$HOME/xscore-dev/xscore_v1.3/parameter/"

OUT="$ROOT/output_all_ligands_xscore.txt"

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

# Same extractor as test_all_ligands.sh. XScore prints "terms VDW=<> HB=<> HP=<> RT=<> ..." once per
# scored ligand; on a pocket error nothing is printed and the fields fall back to "?".
terms_of() {
  # $1 = raw xscore output ; echoes "VDW=<> HB=<> HP=<> RT=<>"
  local text="$1" vdw hb hp rt
  vdw="$(printf '%s\n' "$text" | grep -oE 'VDW=[^ ]+' | head -1)"
  hb="$(printf '%s\n' "$text"  | grep -oE 'HB=[^ ]+'  | head -1)"
  hp="$(printf '%s\n' "$text"  | grep -oE 'HP=[^ ]+'  | head -1)"
  rt="$(printf '%s\n' "$text"  | grep -oE 'RT=[^ ]+'  | head -1)"
  echo "${vdw:-VDW=?} ${hb:-HB=?} ${hp:-HP=?} ${rt:-RT=?}"
}

# Run XScore for a single (protein, ligand) pair and echo its full output.
run_xscore() {
  # $1 = protein pdb, $2 = ligand mol2
  local pdb="$1" lig="$2" inp="$TMP/pair.input"
  cat > "$inp" <<EOF
FUNCTION            SCORE
RECEPTOR_PDB_FILE   $pdb
LIGAND_MOL2_FILE    $lig
OUTPUT_TABLE_FILE   $TMP/pair.table
OUTPUT_LOG_FILE     $TMP/pair.log
NUMBER_OF_HITS      1
HITS_DIRECTORY      $TMP/pair.mdb
SHOW_ATOM_BIND_SCORE    NO
APPLY_HPSCORE       YES
APPLY_HMSCORE       YES
APPLY_HSSCORE       YES
APPLY_CHEMICAL_RULES    NO
EOF
  "$XSCORE" "$inp" 2>&1
}

# --- sanity checks ---
[ -x "$XSCORE" ] || { echo "xscore binary not found at $XSCORE"; exit 1; }
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
    xs="$(terms_of "$(run_xscore "$pdb" "$lig")")"
    printf '    %-10s %s\n' "$lname" "${xs:-<no output>}" >> "$OUT"
  done

  echo >> "$OUT"
done

echo "Done: $((pcount * num_lig)) pair(s) written to $OUT"
