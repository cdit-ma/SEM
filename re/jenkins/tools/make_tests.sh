#!/bin/bash
mkdir -p "python_deleter_tests"
pushd "python_deleter_tests" > "/dev/null" || exit

touch "file.txt"
touch -d "2 days ago" "old_file.txt"
touch -d "10 days ago" "oldest_file.txt"

mkdir -p "dir"
mkdir -p "old_dir"
mkdir -p "oldest_dir"

touch -d "2 days ago" "old_dir"
touch -d "10 days ago" "oldest_dir"
popd > "/dev/null" || exit