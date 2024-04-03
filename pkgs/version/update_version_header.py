#!/usr/bin/env python

import getpass
import socket
import argparse
import os
import sys
import subprocess
import re

parser = argparse.ArgumentParser(
    description="""Extract version info from git and
generate a version header file. The working directory is expected to be
the root of Firmware."""
)
parser.add_argument("filename", metavar="board_fw_version_autogen.h", help="Header output file")
parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="Verbose output", default=False)
parser.add_argument("--validate", dest="validate", action="store_true", help="Validate the tag format", default=False)

args = parser.parse_args()
filename = args.filename
verbose = args.verbose
validate = args.validate

try:
    fp_header = open(filename, "r")
    old_header = fp_header.read()
except:
    old_header = ""


# Generate the header file content
header = """
/* Auto Magically Generated file */
/* Do not edit! */
#ifndef __BOARD_FW_VERSION_AUTOGEN_H__
#define __BOARD_FW_VERSION_AUTOGEN_H__
"""


# Nextpilot
git_describe_cmd = "git describe --exclude ext/* --always --tags --dirty"
git_tag = subprocess.check_output(git_describe_cmd.split(), stderr=subprocess.STDOUT).decode("utf-8").strip()

try:
    # get the tag if we're on a tagged commit
    tag_or_branch = (
        subprocess.check_output((git_describe_cmd + " --exact-match").split(), stderr=subprocess.STDOUT).decode("utf-8").strip()
    )
except:
    tag_or_branch = None

if validate:
    if verbose:
        print("testing git tag: " + git_tag)
    # remove optional '-dirty' at the end
    git_tag_test = re.sub(r"-dirty$", "", git_tag)
    # remove optional -<num_commits>-g<commit_hash> at the end (in case we are not on a tagged commit)
    git_tag_test = re.sub(r"-[0-9]+-g[0-9a-fA-F]+$", "", git_tag_test)
    # now check the version format
    m = re.match(
        r"v([0-9]+)\.([0-9]+)\.[0-9]+(((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|(-rc[0-9]+))|"
        r"(-[0-9]+\.[0-9]+\.[0-9]+((-dev)|(-alpha[0-9]+)|(-beta[0-9]+)|([-]?rc[0-9]+))?))?$",
        git_tag_test,
    )
    if m:
        # format matches, check the major and minor numbers
        major = int(m.group(1))
        minor = int(m.group(2))
        if major < 1 or (major == 1 and minor < 9):
            print("")
            print("Error: NextPilot version too low, expected at least v1.9.0")
            print("Check the git tag (current tag: '{:}')".format(git_tag_test))
            print("")
            sys.exit(1)
    else:
        print("")
        print("Error: the git tag '{:}' does not match the expected format.".format(git_tag_test))
        print("")
        print("The expected format is 'v<Nexpilot version>[-<custom version>]'")
        print("  <Nexpilot version>: v<major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        print("  <custom version>: <major>.<minor>.<patch>[-rc<rc>|-beta<beta>|-alpha<alpha>|-dev]")
        print("Examples:")
        print("  v1.9.0-rc3 (preferred)")
        print("  v1.9.0-beta1")
        print("  v1.9.0-1.0.0")
        print("  v1.9.0-1.0.0-alpha2")
        # print("See also https://dev.px4.io/master/en/setup/building_px4.html#firmware_version")
        print("")
        sys.exit(1)

git_version = subprocess.check_output("git rev-parse --verify HEAD".split(), stderr=subprocess.STDOUT).decode("utf-8").strip()
try:
    git_branch_name = (
        subprocess.check_output("git symbolic-ref -q --short HEAD".split(), stderr=subprocess.STDOUT).decode("utf-8").strip()
    )
except:
    git_branch_name = ""
git_version_short = git_version[0:16]

# OEM version
try:
    oem_tag = (
        subprocess.check_output("git describe --match ext/oem-* --tags".split(), stderr=subprocess.STDOUT)
        .decode("utf-8")
        .strip()
    )
    oem_tag = oem_tag[8:]
except:
    oem_tag = ""

if tag_or_branch is None:
    # replace / so it can be used as directory name
    tag_or_branch = git_branch_name.replace("/", "-")
    # either a release or master branch (used for metadata)
    if not tag_or_branch.startswith("release-"):
        tag_or_branch = "master"

header += """
#define FW_GIT_HASH_STR "{git_version}"
#define FW_GIT_HASH_HEX 0x{git_version_short}
#define FW_GIT_TAG_STR "{git_tag}"
#define FW_GIT_BRANCH_STR "{git_branch_name}"
#define FW_GIT_OEM_TAG_STR "{oem_tag}"
#define FW_GIT_TAG_OR_BRANCH "{tag_or_branch}" // special variable: git tag, release or master branch
""".format(
    git_version=git_version,
    git_version_short=git_version_short,
    git_tag=git_tag,
    git_branch_name=git_branch_name,
    oem_tag=oem_tag,
    tag_or_branch=tag_or_branch,
)


# ECL
if os.path.exists("src/lib/ecl/.git"):
    ecl_git_tag = subprocess.check_output(
        "git describe --always --tags --dirty".split(), cwd="src/lib/ecl", stderr=subprocess.STDOUT
    ).decode("utf-8")

    ecl_git_version = (
        subprocess.check_output("git rev-parse --verify HEAD".split(), cwd="src/lib/ecl", stderr=subprocess.STDOUT)
        .decode("utf-8")
        .strip()
    )
    ecl_git_version_short = ecl_git_version[0:16]

    header += """
#define ECL_GIT_VERSION_STR "{ecl_git_version}"
#define ECL_GIT_VERSION_BINARY 0x{ecl_git_version_short}
""".format(
        ecl_git_version=ecl_git_version, ecl_git_version_short=ecl_git_version_short
    )


# Mavlink
if os.path.exists("mavlink/include/mavlink/v2.0/.git"):
    mavlink_git_version = (
        subprocess.check_output(
            "git rev-parse --verify HEAD".split(), cwd="mavlink/include/mavlink/v2.0", stderr=subprocess.STDOUT
        )
        .decode("utf-8")
        .strip()
    )
    mavlink_git_version_short = mavlink_git_version[0:16]

    header += """
#define MAVLINK_GIT_HASH_STR "{mavlink_git_version}"
#define MAVLINK_GIT_HASH_BIN 0x{mavlink_git_version_short}
""".format(
        mavlink_git_version=mavlink_git_version, mavlink_git_version_short=mavlink_git_version_short
    )
else:
    header += """
#define MAVLINK_GIT_HASH_STR "f03d91ae932dd718d81f851b584dbd21cffafdd2"
#define MAVLINK_GIT_HASH_BIN 0xf03d91ae932dd718
"""


# RT-Thread
if os.path.exists("rots/rt-thread/.git"):
    os_git_tag = (
        subprocess.check_output(
            "git describe --always --tags --match nuttx-* --dirty".split(),
            cwd="platforms/nuttx/NuttX/nuttx",
            stderr=subprocess.STDOUT,
        )
        .decode("utf-8")
        .strip()
        .replace("nuttx-", "v")
    )
    os_git_tag = re.sub("-.*", ".0", os_git_tag)
    os_git_version = (
        subprocess.check_output(
            "git rev-parse --verify HEAD".split(), cwd="platforms/nuttx/NuttX/nuttx", stderr=subprocess.STDOUT
        )
        .decode("utf-8")
        .strip()
    )
    os_git_version_short = os_git_version[0:16]

    header += """
#define OS_GIT_HASH_STR "{os_git_version}"
#define OS_GIT_HASH_HEX 0x{os_git_version_short}
#define OS_GIT_TAG_STR "{os_git_tag}"
""".format(
        os_git_version=os_git_version, os_git_version_short=os_git_version_short, os_git_tag=os_git_tag
    )
else:
    header += """
#define OS_GIT_HASH_STR "aab2428d4177a02cd3b0fd020e47a88de379a6ab"
#define OS_GIT_HASH_HEX 0xaab2428d4177a02c
#define OS_GIT_TAG_STR "v5.0.2"
#define OS_GIT_BRANCH_STR "v5.0.2"
"""

# URI
host_name = socket.gethostname()
host_ip = socket.gethostbyname(socket.gethostname())
user_name = getpass.getuser()
header += """
#define FW_BUILD_URI "{user_name}@{host_ip}({host_name})"
""".format(
    host_name=host_name, user_name=user_name, host_ip=host_ip
)


header += """
#endif // __BOARD_FW_VERSION_AUTOGEN_H__
"""

if old_header != header:
    if verbose:
        print("Updating header {}".format(filename))
    fp_header = open(filename, "w")
    fp_header.write(header)
