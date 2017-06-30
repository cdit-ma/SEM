# Install script for directory: C:/MEDEA/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/MEDEA")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "MEDEA" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/MEDEA" TYPE EXECUTABLE FILES "C:/MEDEA/build-src-Desktop_Qt_5_7_0_MSVC2015_64bit-Default/bin/MEDEA.exe")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "RE_GEN" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/MEDEA/Resources" TYPE DIRECTORY FILES "C:/MEDEA/src/../re_gen" FILES_MATCHING REGEX "/[^/]*\\.xsl$" REGEX "/[^/]*\\.jar$")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "SCRIPTS" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/MEDEA/Resources" TYPE DIRECTORY FILES "C:/MEDEA/src/../scripts" FILES_MATCHING REGEX "/[^/]*\\.[^/]*$")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "QT_LIBS" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/MEDEA/" TYPE DIRECTORY FILES "C:/MEDEA/build-src-Desktop_Qt_5_7_0_MSVC2015_64bit-Default/bin/" FILES_MATCHING REGEX "/[^/]*\\.dll$" REGEX "/[^/]*\\.qm$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/MEDEA/build-src-Desktop_Qt_5_7_0_MSVC2015_64bit-Default/ModelController/cmake_install.cmake")
  include("C:/MEDEA/build-src-Desktop_Qt_5_7_0_MSVC2015_64bit-Default/Test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/MEDEA/build-src-Desktop_Qt_5_7_0_MSVC2015_64bit-Default/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
