#! /bin/bash

set -e

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

# Use colordiff if available
if command -v colordiff >/dev/null 2>&1; then
    diff_cmd=colordiff
else
    diff_cmd=diff
fi

# JSON files
pushd "$DIR"/..

function check_json_format() {
	json="$1"
	echo "Checking format for $json"
	cp $json $json.tmp
	./scripts/format_json.sh $json.tmp
    if ! cmp $json $json.tmp >/dev/null 2>&1; then
        echo "Changed $json:"
        $diff_cmd $json.tmp $json || true
		echo "Run ./scripts/format_json.sh $json"
		exit 1
    fi
	rm $json.tmp
}

json_files=(events/common.json libs/test.json)
for json in ${json_files[@]}; do
	check_json_format "$json"
	echo "Validating $json"
	./scripts/validate.py $json
done
check_json_format validation/schema.json
check_json_format validation/schema.translation.json

./scripts/generate_all.sh --check || (echo -e "Run\n./scripts/generate_all.sh\nand commit the changes\n" && exit -1)

./scripts/run_pylint.sh

popd

# C++
pushd "$DIR"/../libs/cpp
echo "Running C++ tests"
BUILD_DIR=build_ci
[ ! -d "$BUILD_DIR" ] && mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ..
make
echo "Running Tests"
./tests/parser_test
echo "Checking source format"
make format || (echo -e "\nFormat check failed, run:\n(mkdir -p libs/cpp/build && cd libs/cpp/build && cmake .. && make format)\n" && exit 1)
popd

# Python
pushd "$DIR"/../libs/python
echo "Running Python tests"
./tests/parser.py
./tests/health_and_arming_checks.py
popd

exit 0

