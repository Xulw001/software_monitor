cmake_minimum_required (VERSION 3.20)
project (SoftwareMonitor LANGUAGES C CXX)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH "x64")
else()
    set(ARCH "x86")
endif()

if(NOT TARGET_DIRECTORY)
    set(TARGET_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
endif()

if(NOT INSTALL_DIRECTORY)
    set(INSTALL_DIRECTORY ${CMAKE_SOURCE_DIR}/install)
endif()

# Set Target Output Path
set(BIN_OUTPUT_DIR ${TARGET_DIRECTORY}/bin/${ARCH})
set(LIB_OUTPUT_DIR ${TARGET_DIRECTORY}/lib/${ARCH})

# Set Default Target Output Path
foreach(config DEBUG RELEASE RELWITHDEBINFO)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} ${BIN_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config} ${LIB_OUTPUT_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config} ${LIB_OUTPUT_DIR})
endforeach()

link_directories(${CMAKE_SOURCE_DIR}/lib/${ARCH})
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/lpc)

set(KRTL_ROOT ${CMAKE_SOURCE_DIR}/include)

include(${KRTL_ROOT}/krtl/wdk.cmake)
wdk_clean_default_options()
add_subdirectory(src/drv)
wdk_restore_default_options()

add_subdirectory(src/srv)

install(DIRECTORY lib/x64/ DESTINATION ${INSTALL_DIRECTORY} FILES_MATCHING PATTERN "*.dll")
install(DIRECTORY ${BIN_OUTPUT_DIR}/ DESTINATION ${INSTALL_DIRECTORY} FILES_MATCHING PATTERN "*.exe" PATTERN "*.sys")
