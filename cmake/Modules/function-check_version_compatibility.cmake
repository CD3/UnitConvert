# A function to parse verson numbers into their parts
include("${CMAKE_CURRENT_LIST_DIR}/function-parse_version.cmake")

function( CHECK_VERSION_COMPATIBILITY PACKAGE_CONFIG VERSION RESULT)
# checks to see if a given version number is compatible with a given package, using
# the packages *ConfigVersion.cmake file.

set( ${RESULT} "CONFIGVERSION-NOTFOUND" PARENT_SCOPE )

string( REGEX REPLACE "Config\.cmake$" "ConfigVersion.cmake" PACKAGE_CONFIGVERSION ${PACKAGE_CONFIG} )
if( EXISTS "${PACKAGE_CONFIGVERSION}" )
  parse_version( ${VERSION} PACKAGE_FIND_ )
  include("${PACKAGE_CONFIGVERSION}")
  set( ${RESULT} ${PACKAGE_VERSION_COMPATIBLE} PARENT_SCOPE )
endif()

endfunction(CHECK_VERSION_COMPATIBILITY)

