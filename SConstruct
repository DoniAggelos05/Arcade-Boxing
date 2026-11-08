#!/usr/bin/env python

# Χτίζει πρώτα το godot-cpp για το Godot 4.7, και μας δίνει το "περιβάλλον" του
env = SConscript("godot-cpp/SConstruct", {"api_version": "4.7"})

# Ο δικός μας κώδικας: όλα τα .cpp στον φάκελο src/
env.Append(CPPPATH=["src/"])
if env.get("is_msvc", False):
    env.Append(CCFLAGS=["/utf-8"])
import os
sources = []
for root, dirs, files in os.walk("src"):
    sources += Glob(os.path.join(root, "*.cpp"))

# Το αποτέλεσμα: π.χ. game/bin/libarcadeboxing.windows.template_debug.x86_64.dll
library = env.SharedLibrary(
    "game/bin/libarcadeboxing{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=sources,
)

env.NoCache(library)
Default(library)
