#! /bin/bash

root=$(git rev-parse --show-toplevel)
test_dir="${root}/conan_package-test.d"

function exit_on_error()
{
  echo "There was an error."
  rm -rf "${test_dir}"
}

trap exit_on_error ERR

set -e

cd "$root"
conan create . local/testing
mkdir "${test_dir}"
cd "${test_dir}"
conan install ../testing/conan_package/test_package
conan build ../testing/conan_package/test_package
./example
cd "$root"
rm -rf "${test_dir}"
