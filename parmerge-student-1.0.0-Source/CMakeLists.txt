
cmake_minimum_required(VERSION 3.0)
   
project(parmerge LANGUAGES C)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")

find_package(OpenMP REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")

option(ENABLE_ADMIN "enable admin mode" off)

if(ENABLE_ADMIN)

set(MERGE_CODE1 ${SRC_DIR}/merge1_solution.c)
set(MERGE_CODE2 ${SRC_DIR}/merge2_solution.c)
set(MERGE_CODE3 ${SRC_DIR}/merge3_solution.c)

else(ENABLE_ADMIN)

set(MERGE_CODE1 ${SRC_DIR}/merge1.c)
set(MERGE_CODE2 ${SRC_DIR}/merge2.c)
set(MERGE_CODE3 ${SRC_DIR}/merge3.c)

endif(ENABLE_ADMIN)

add_executable(merge1_tester
        ${MERGE_CODE1}
        ${SRC_DIR}/seq_common.c
        ${SRC_DIR}/parmerge_tester.c
        )

add_executable(merge2_tester
        ${MERGE_CODE2}
        ${SRC_DIR}/seq_common.c
        ${SRC_DIR}/parmerge_tester.c
        )

add_executable(merge3_tester
        ${MERGE_CODE3}
        ${SRC_DIR}/seq_common.c
        ${SRC_DIR}/parmerge_tester.c
        )

target_link_libraries(merge1_tester m)
target_link_libraries(merge2_tester m)
target_link_libraries(merge3_tester m)

set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_GENERATOR "TBZ2")

set(MY_IGNORE_FILES
"/CMakeFiles/"
"/_CPack_Packages/"
"/bin/"
"/Makefile"
"\\\\.tar\\\\.bz2$"
"/\\\\.git/"
"/\\\\.DS_Store"
".project"
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

set(CPACK_PACKAGE_NAME "parmerge-admin")

else(ENABLE_ADMIN)

SET(CPACK_SOURCE_IGNORE_FILES
${MY_IGNORE_FILES}
"/src/.*_solution.c"
)

set(CPACK_PACKAGE_NAME "parmerge-student")

endif(ENABLE_ADMIN)

INCLUDE(CPack)

