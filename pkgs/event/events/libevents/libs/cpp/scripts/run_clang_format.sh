#!/usr/bin/env bash

# Script to run clang-format and show diff.
#
# Usage: ./run_clang_format.sh /usr/bin/clang-format src/a.cpp src/b.h ...

# Keep clang-format executable for later.
clang_format_executable=$1

# Get remaining args
shift

# Use colordiff if available
if command -v colordiff >/dev/null 2>&1; then
    diff_cmd=colordiff
else
    diff_cmd=diff
fi

# Keep track of errors for the exit value
error_found=false

# Go through files and run clang-format in place and show diff against .orig file.
for file in $@; do

    cp $file $file.orig
    result=`clang-format -style=file -i $file`

    if ! cmp $file $file.orig >/dev/null 2>&1; then
        echo "Changed $file:"
        $diff_cmd $file.orig $file
        error_found=true
    fi

    rm $file.orig
done

if [ "$error_found" = true ]; then
    exit 1
fi
