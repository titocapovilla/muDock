import json
import sys
import pathlib

# define path to stuff
script_dirpath = pathlib.Path(__file__).parent
dat_filepath = script_dirpath.parent.parent / "chem" / "ATOM_DEF_XTOOL.dat"
with open(dat_filepath, "r") as f:
    lines = f.readlines()
    data = []
    for line in lines:
        parts = line.split()
        # if len(parts) < 7:
        #     continue
        data.append(
            {
                "value": parts[1],
                "name": parts[1],
                "atomic_weight": parts[2],
                "vdw_radius": parts[3],
                "vdw_potential": parts[4],
                "par_charge": parts[5],
                "hbond": parts[6],
            }
        )

    print(json.dumps(data, indent=4))
