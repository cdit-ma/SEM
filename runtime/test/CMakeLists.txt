cmake_minimum_required (VERSION 2.6)

set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/testrxmessage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/testtxmessage.cpp
   PARENT_SCOPE
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/testrxmessage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/testtxmessage.h
   PARENT_SCOPE
)