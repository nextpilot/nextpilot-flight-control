#!/usr/bin/env python
# /*****************************************************************
#  *     _   __             __   ____   _  __        __
#  *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
#  *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
#  *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
#  * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
#  *
#  * Copyright All Reserved © 2015-2024 NextPilot Development Team
#  ******************************************************************/

#
# nextpilot firmware image generator
#
# The nextpilot firmware file is a JSON-encoded Python object, containing
# metadata fields and a zlib-compressed base64-encoded firmware image.
#

import getpass
import socket
import argparse
import json
import base64
import zlib
import time
import subprocess

#
# Construct a basic firmware description
#


def mkdesc():
    proto = {}
    proto["magic"] = "PX4FWv1"
    proto["board_id"] = 0
    proto["board_revision"] = 0
    proto["version"] = ""
    proto["summary"] = ""
    proto["description"] = ""
    proto["git_identity"] = ""  # git tag
    proto["git_hash"] = ""  # git commit hash
    proto["git_branch"] = ""  # git branch
    proto["build_time"] = 0
    proto["build_uri"] = ""
    proto["image_size"] = 0
    proto["image"] = bytes()
    return proto


# Parse commandline
parser = argparse.ArgumentParser(description="Firmware generator for the PX autopilot system.")
parser.add_argument("--prototype", action="store", help="read a prototype description from a file")
parser.add_argument("--board_id", action="store", help="set the board ID required")
parser.add_argument("--board_revision", action="store", help="set the board revision required")
parser.add_argument("--version", action="store", help="set a version string")
parser.add_argument("--summary", action="store", help="set a brief description")
parser.add_argument("--description", action="store", help="set a longer description")
parser.add_argument("--git_identity", action="store", help="the working directory to check for git identity")
parser.add_argument("--parameter_xml", action="store", help="the parameters.xml file")
parser.add_argument("--airframe_xml", action="store", help="the airframes.xml file")
parser.add_argument("--image", action="store", help="the firmware image")
args = parser.parse_args()

# Fetch the firmware descriptor prototype if specified
if args.prototype != None:
    f = open(args.prototype, "r")
    desc = json.load(f)
    f.close()
else:
    desc = mkdesc()

desc["build_time"] = int(time.time())

host_name = socket.gethostname()
host_ip = socket.gethostbyname(socket.gethostname())
user_name = getpass.getuser()
desc["build_uri"] = host_name + "@" + host_ip + "(" + host_name + ")"

if args.board_id != None:
    desc["board_id"] = int(args.board_id)
if args.board_revision != None:
    desc["board_revision"] = int(args.board_revision)
if args.version != None:
    desc["version"] = str(args.version)
if args.summary != None:
    desc["summary"] = str(args.summary)
if args.description != None:
    desc["description"] = str(args.description)
if True:  # args.git_identity != None:
    cmd = "git describe --exclude ext/* --always --tags --dirty"
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE).stdout
    desc["git_identity"] = p.read().strip().decode("utf-8")
    p.close()
    cmd = "git symbolic-ref -q --short HEAD"
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE).stdout
    desc["git_branch"] = p.read().strip().decode("utf-8")
    p.close()
    cmd = "git rev-parse --verify HEAD"
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE).stdout
    desc["git_hash"] = p.read().strip().decode("utf-8")
    p.close()
if args.parameter_xml != None:
    f = open(args.parameter_xml, "rb")
    bytes = f.read()
    desc["parameter_xml_size"] = len(bytes)
    desc["parameter_xml"] = base64.b64encode(zlib.compress(bytes, 9)).decode("utf-8")
    desc["mav_autopilot"] = 12  # 12 = MAV_AUTOPILOT_PX4
if args.airframe_xml != None:
    f = open(args.airframe_xml, "rb")
    bytes = f.read()
    desc["airframe_xml_size"] = len(bytes)
    desc["airframe_xml"] = base64.b64encode(zlib.compress(bytes, 9)).decode("utf-8")
if args.image != None:
    f = open(args.image, "rb")
    bytes = f.read()
    desc["image_size"] = len(bytes)
    desc["image"] = base64.b64encode(zlib.compress(bytes, 9)).decode("utf-8")

print(json.dumps(desc, indent=4))
