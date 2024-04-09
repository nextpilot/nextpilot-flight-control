#! /bin/bash
# Format one or more json files

[[ "$#" == 0 || "$1" == "-h" || "$1" == "--help" ]] \
	&& echo "Usage: $0 <json_file> [<json_file>...]" && exit 0

set -e


for file in "$@"
do
	python -m json.tool "$file" > "$file".tmp
	mv "$file".tmp "$file"
done

exit 0

