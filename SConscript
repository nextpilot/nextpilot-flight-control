import os
import rtconfig
from building import *

objs = []
cwd = GetCurrentDir()

for d in os.listdir(cwd):
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, "SConscript")):
        objs += SConscript(os.path.join(d, "SConscript"), variant_dir="#/build/" + d, duplicate=0)

Return("objs")
