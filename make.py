import os
import os.path as osp


REPOSITORY_ROOT = os.getcwd()

AIMBOT_ROOT = osp.join(REPOSITORY_ROOT, "aimbot")
PHOTONIC_ROOT = osp.join(REPOSITORY_ROOT, "photonic")
INCLUDE_ROOT = osp.join(REPOSITORY_ROOT, "include")
SRC_ROOT = osp.join(REPOSITORY_ROOT, "src")

MAKE_SOURCE_ROOTS = [
    AIMBOT_ROOT,
    PHOTONIC_ROOT,
    INCLUDE_ROOT,
    SRC_ROOT
]
MAKE_SOURCE_EXTENSIONS = [
    ".cpp",
    ".cc",
    ".hpp",
    ".h"
]
MAKE_SUBSTRING_BLACKLIST = [
    "utest",
    "legacy"
]
MAKE_RECIPE_NAME = "ff"
MAKE_MAIN_NAME = "flight_factory.cpp"
MAKE_FLAGS = ["-std=c++11"]
MAKE_RECIPE_STEM = "g++ " + " ".join(MAKE_FLAGS) + " " +                       \
                   osp.join(SRC_ROOT, MAKE_MAIN_NAME) +                        \
                   " -o " + MAKE_RECIPE_NAME

makefile = open("Makefile", "w")


def append_src(root, fname):
    makefile.write(" \\\n\t" + osp.join(root, fname))

def append_include(root, dirname):
    makefile.write(" \\\n\t-I" + osp.join(root, dirname))

def judge(root, fname, dir=False):
    path = osp.join(root, fname)
    fstem, fext = os.path.splitext(path)

    # File-only inclusion rules
    if not dir:
        if fext not in MAKE_SOURCE_EXTENSIONS:
            return
        elif fname == MAKE_MAIN_NAME:
            return

    blacklisted = False

    # Determine if item is in a blacklisted directory
    for blackstr in MAKE_SUBSTRING_BLACKLIST:
        if blackstr in str(path):
            return

    if dir:
        append_include(root, fname)
    else:
        append_src(root, fname)


# Main recipe
makefile.write("make " + MAKE_RECIPE_NAME + ":" + "\n")
makefile.write("\t" + MAKE_RECIPE_STEM)

# Compile list of all source files
for srcroot in MAKE_SOURCE_ROOTS:
    for root, dirs, files in os.walk(srcroot):
        for file in files:
            judge(root, file)

        for dir in dirs:
            judge(root, dir, True)

# Cleanup macro
makefile.write("\n\nmake clean:\n\trm " + MAKE_RECIPE_NAME)

makefile.close()
