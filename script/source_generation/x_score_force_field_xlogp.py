import json
import sys
import pathlib

# define path to stuff
script_dirpath = pathlib.Path(__file__).parent
dat_filepath = script_dirpath.parent.parent / "chem" / "ATOM_DEF_XLOGP.dat"
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
                "hbond": parts[2],
                "hydrophobic_scale": parts[3],
            }
        )

    print(json.dumps(data, indent=4))
