#! /bin/bash

set -e

if [[ -z $1 ]]
then
  echo "usage: $0 <NAME>"
  exit 1
fi

name=$1
shift

tempdir=$(dirname $(dirname $0))

git clone $tempdir $name
cd $name
mv .git .git-projecttemplate

git init
git add .
git commit -m "initial import"
git tag v0.0.0
