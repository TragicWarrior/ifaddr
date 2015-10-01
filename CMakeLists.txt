cmake_minimum_required(VERSION 2.8)

project(ifaddr)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

if (("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") OR (("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU") AND (CMAKE_C_COMPILER_VERSION VERSION_GREATER 4.0)))
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
endif()

# the following is until we learn how to reorder the gcc arguments to correctly link on Ubuntu
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")

add_executable(ifaddr
    main.c
    )

# set the subst vars that configure_file() will use
set(PROJECT_VERSION 1.0)

install (TARGETS ifaddr DESTINATION bin)
