#! /bin/bash

set -e

if [[ -z ${1} ]]
then
  echo "usage: $0 <PROJECT_NAME>"
  exit 1
fi

PROJECT_NAME=$1
shift


# update CMakeLists.txt with project name
sed -i "s/TemplateProject/${PROJECT_NAME}/g" CMakeLists.txt

# rename version file
[[ -e src/TemplateProject_version.h.in ]] && mv src/TemplateProject_version.h.in src/${PROJECT_NAME}_version.h.in
rm -f src/RemoveMe.*
