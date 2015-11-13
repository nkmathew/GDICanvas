cmake_minimum_required(VERSION 3.0)

set(LIB_NAME GDICanvas)
project(${LIB_NAME} CXX)

set(CMAKE_CXX_COMPILER gcc)
set(CMAKE_MAKE_PROGRAM mingw32-make)

# File locations
set(SRC_DIR src)
set(INC_DIR include)
set(BIN_DIR ../../build)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/bin)

# Flags for MinGW
IF(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-lstdc++ -Wall -pedantic -Wextra -std=c++11 -mwindows -lwinmm")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

set_source_files_properties(logo.ico logo.rc PROPERTIES LANGUAGE RC)

if(MINGW)
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT
        "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

message(${CMAKE_CXX_FLAGS})

set(EXECUTABLE_NAME test)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Name of test file
set(TEST test)

# Source Files
set(CXX_FILES
    ${SRC_DIR}/Canvas.cxx
    ${SRC_DIR}/Colors.cxx
    ${SRC_DIR}/Shapes.cxx
    ${SRC_DIR}/Vec2D.cxx
    ${SRC_DIR}/logo.rc
    )

# Include files. To be copied to the build folder
set(INCLUDE_FILES
    src/Canvas.h
    src/Colors.h
    src/Shapes.h
    src/Vec2D.h
    src/VirtualKeys.h
    src/logo.h
    )

# Include files. To be copied to the build folder
set(H_FILES
    ${INC_DIR}/build/include/Canvas.h
    ${INC_DIR}/build/include/Colors.h
    ${INC_DIR}/build/include/Shapes.h
    ${INC_DIR}/build/include/Vec2D.h
    ${INC_DIR}/build/include/VirtualKeys.h
    ${INC_DIR}/build/include/logo.h
    )

# add_subdirectory(examples)

# add_library (core OBJECT ${CXX_FILES})
# add_library(${LIBRARY_NAME} STATIC ${CXX_FILES} ${INCLUDE_FILES})
add_library(${LIB_NAME} STATIC ${CXX_FILES} ${INCLUDE_FILES})
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# add_executable(${EXECUTABLE_NAME} test.cxx)
# target_link_libraries(${TEST} ${add_LIBS})
