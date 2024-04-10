#! /bin/bash
# execute pylint on the source

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
pylint_exec=$(which pylint 2>/dev/null)

[[ $? != 0 ]] && { echo >&2 "pylint not found. Aborting."; exit 1; }

set -e

cd "$DIR"
python $pylint_exec --rcfile=../pylintrc *.py \
	../libs/python/libevents_parse/*.py \
	../libs/python/tests/*.py

exit 0
