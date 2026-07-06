#!/usr/bin/env bash
#
# Runs every ligand/protein pair found in the muDock test sets through both muDock's X-Score
# (x_score_bench) and the original XScore, writing a side-by-side comparison of the raw terms that are
# currently implemented (VDW HP RT) to output.txt.
#
# Terms are being re-introduced one at a time; extend the *_RE regexes below as new terms come online.
#
# Usage:  ./test.sh
#
set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# --- locations (edit here if your layout differs) ---
PROT_DIR="$HOME/xscore-dev/srcFiles/proteins/mudock_proteins"
LIG_DIR="$HOME/xscore-dev/srcFiles/ligands/mudock_ligands"
XTOOL_DIR="$HOME/xscore-dev/srcFiles/xtool"           # holds the score_<name>.input files
BENCH="$ROOT/build/application/bench/x_score_bench"
XSCORE="$HOME/xscore-dev/xscore_v1.3/c++/xscore"
export XSCORE_PARAMETER="$HOME/xscore-dev/xscore_v1.3/parameter/"

OUT="$ROOT/output.txt"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

# Extract the currently-implemented terms (VDW, HP, RT) from a tool's output. Each term is printed by
# muDock and XScore as "VDW=<value>" / "HP=<value>" / "RT=<value>"; we pull them individually so the two
# tools can print different sets of terms (XScore also emits HB/HM/HS) without breaking the comparison.
terms_of() {
  # $1 = raw multi-line tool output ; echoes "VDW=<> HP=<> RT=<>"
  local text="$1" vdw hp rt
  vdw="$(printf '%s\n' "$text" | grep -oE 'VDW=[^ ]+' | head -1)"
  hp="$(printf '%s\n' "$text"  | grep -oE 'HP=[^ ]+'  | head -1)"
  rt="$(printf '%s\n' "$text"  | grep -oE 'RT=[^ ]+'  | head -1)"
  echo "${vdw:-VDW=?} ${hp:-HP=?} ${rt:-RT=?}"
}

# --- sanity checks ---
if [ ! -x "$BENCH" ]; then
  echo "x_score_bench not found at $BENCH — building it ..."
  cmake --build "$ROOT/build" --target x_score_bench -j4 || { echo "build failed"; exit 1; }
fi
[ -x "$XSCORE" ] || { echo "xscore binary not found at $XSCORE"; exit 1; }
[ -d "$PROT_DIR" ] || { echo "protein dir not found: $PROT_DIR"; exit 1; }
[ -d "$LIG_DIR" ]  || { echo "ligand dir not found: $LIG_DIR"; exit 1; }

: > "$OUT"

run_xscore() {
  # $1 = name, $2 = protein pdb, $3 = ligand mol2 ; echoes the full xscore output
  local name="$1" pdb="$2" lig="$3" inp
  if [ -f "$XTOOL_DIR/score_${name}.input" ]; then
    # use the validated input (relative paths -> run from its directory)
    ( cd "$XTOOL_DIR" && "$XSCORE" "score_${name}.input" ) 2>&1
  else
    # generate a minimal input for this pair (defaults for the coefficients)
    inp="$TMP/${name}.input"
    cat > "$inp" <<EOF
FUNCTION            SCORE
RECEPTOR_PDB_FILE   $pdb
LIGAND_MOL2_FILE    $lig
OUTPUT_TABLE_FILE   $TMP/${name}.table
OUTPUT_LOG_FILE     $TMP/${name}.log
NUMBER_OF_HITS      1
HITS_DIRECTORY      $TMP/${name}.mdb
SHOW_ATOM_BIND_SCORE    NO
APPLY_HPSCORE       YES
APPLY_HMSCORE       YES
APPLY_HSSCORE       YES
APPLY_CHEMICAL_RULES    NO
EOF
    "$XSCORE" "$inp" 2>&1
  fi
}

count=0
for pdb in "$PROT_DIR"/*_protein.pdb; do
  [ -e "$pdb" ] || continue
  name="$(basename "$pdb" _protein.pdb)"
  lig="$LIG_DIR/${name}_ligand.mol2"
  if [ ! -f "$lig" ]; then
    echo "skip $name (no ligand at $lig)"
    continue
  fi

  echo "scoring $name ..."
  md="$(terms_of "$("$BENCH" -p "$pdb" -l "$lig" 2>&1)")"
  xs="$(terms_of "$(run_xscore "$name" "$pdb" "$lig")")"

  {
    echo "$name"
    echo "    mudock    ${md:-<no output>}"
    echo "    xscore    ${xs:-<no output>}"
    echo
  } >> "$OUT"
  count=$((count + 1))
done

echo "Done: $count pair(s) written to $OUT"