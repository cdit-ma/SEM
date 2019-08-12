#!/usr/bin/env bash

build_image() {
  current_dir=$1
  image_name=$2
  tag_name=$3

  if [ -z "$tag_name" ]; then
      full_image_name="cdit-ma/$image_name"
    else
      full_image_name="cdit-ma/$image_name:$tag_name"
  fi

  echo "## Building $full_image_name"
  docker build -t "$full_image_name" -f "$current_dir"/"$image_name"/Dockerfile "$current_dir/.."
  echo "## Finished building $full_image_name"
  echo
}

# Any subsequent commands which fail will cause the script to exit immediately
set -eo pipefail

# Get directory of script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
# ARG1 = branch name to build images for
branch_name="$1"

echo "# Docker file location: " "$DIR"
echo "# Building docker images for branch: $branch_name"
echo

build_image "$DIR" minimal_deps
build_image "$DIR" full_deps
build_image "$DIR" re_minimal "$branch_name"
build_image "$DIR" re_full "$branch_name"
build_image "$DIR" aggregation_server "$branch_name"
