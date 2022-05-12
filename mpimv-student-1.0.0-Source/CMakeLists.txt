
cmake_minimum_required(VERSION 3.0)
   
project(mvmpi LANGUAGES C)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")

find_package(MPI REQUIRED)

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)
SET(CMAKE_C_FLAGS "-O3 -Wall -std=c99 -D_POSIX_C_SOURCE=200809L") 
SET(CMAKE_CXX_FLAGS "-O3 -std=c++11")

option(ENABLE_ADMIN "enable admin mode" off)

if(ENABLE_ADMIN)

set(MV1_CODE
${SRC_DIR}/mv1_solution.c
)
set(MV2_CODE
${SRC_DIR}/mv2_solution.c
)

else(ENABLE_ADMIN)

set(MV1_CODE
${SRC_DIR}/mv1.c
)
set(MV2_CODE
${SRC_DIR}/mv2.c
)

endif(ENABLE_ADMIN)


add_executable(mv1
${MV1_CODE}
${SRC_DIR}/mpimv_tester.c
${SRC_DIR}/mv1_instance.c
${SRC_DIR}/utils.c
)

target_link_libraries(mv1 m)

add_executable(mv2
${MV2_CODE}
${SRC_DIR}/mpimv_tester.c
${SRC_DIR}/mv2_instance.c
${SRC_DIR}/utils.c
)

target_link_libraries(mv2 m)

set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_GENERATOR "TBZ2")


set(MY_IGNORE_FILES
"/\\\\.gitignore$"
"/CMakeFiles/"
"/_CPack_Packages/"
"/bin/"
"/Makefile"
"\\\\.tar\\\\.bz2$"
"/\\\\.git/"
"/\\\\.settings/"
"/\\\\.DS_Store"
"cmake_install\\\\.cmake"
"CPackConfig\\\\.cmake"
"CPackSourceConfig\\\\.cmake"
"CMakeCache.txt"
"${CPACK_SOURCE_IGNORE_FILES}"
)

if(ENABLE_ADMIN)

SET(CPACK_SOURCE_IGNORE_FILES
${MY_IGNORE_FILES}
)
set(CPACK_PACKAGE_NAME "mpimv-admin")

else(ENABLE_ADMIN)

SET(CPACK_SOURCE_IGNORE_FILES
${MY_IGNORE_FILES}
"/src/.*_solution.c"
)
set(CPACK_PACKAGE_NAME "mpimv-student")

endif(ENABLE_ADMIN)

INCLUDE(CPack)
