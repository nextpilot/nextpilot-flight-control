#! /bin/bash
# generate outputs from all templates

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd "$DIR"/..
json_file=events/common.json

check=0
out_dir=libs/cpp/generated
if [ "$1" = "--check" ]; then
	out_dir=libs/cpp/generated.tmp
	[ ! -d $out_dir ] && mkdir $out_dir
	check=1
fi

echo "Generating C++ output to $out_dir"
./scripts/generate.py --language cpp --output $out_dir $json_file

if [ $check = 1 ]; then
	for f in $out_dir/*; do
		filename=$(basename -- "$f")
		if ! cmp $f libs/cpp/generated/$filename >/dev/null 2>&1; then
			echo "Found difference in $filename"
			exit -1
		fi
	done
	rm -r $out_dir
fi

