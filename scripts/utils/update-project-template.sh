#! /bin/bash

set -e

if [[ -z "$1" ]]
then
  echo "usage: $0 <SOURCE>"
  exit 1
fi


if [[ -n $(git status --porcelain) ]]
then
  echo "ERROR: working directory is not clean"
  echo "please clean or stash your changes."
  exit 1
fi


src="${1}"
shift
dest=$(git rev-parse --show-toplevel)

echo "pulling template files from '$src' into '$dest'"

rsync --recursive --links "$src/template.0/" "$dest"
