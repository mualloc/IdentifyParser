﻿# CMakeList.txt : CMake project for IdentifyParser, include source and define
# project specific logic here.
#

# Add source to this project's library.
add_library (IdentifyParserLib STATIC "IdentifyParser.h" "IdentifyParser.cpp" "IdentifyDeviceData.h")

target_include_directories(IdentifyParserLib PUBLIC .)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET IdentifyParserLib PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
