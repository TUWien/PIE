# check cmake requirements
cmake_minimum_required(VERSION 3.0)

######################## begin of project
project(PIE)

set(PIE_VERSION 0.1)
add_definitions(-DPIE_FRAMEWORK_VERSION="${PIE_FRAMEWORK_VERSION}")

if(CMAKE_CL_64)
	set(PIE_ARCHITECTURE "x64")
else()
	set(PIE_ARCHITECTURE "x86")
endif()

if (DEFINED GLOBAL_READ_BUILD)
	# set output path when building for READ
	if (MSVC)
		set(CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
	else()
		set(CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/READ)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/READ)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/READ)	
	endif()
endif()

# include macros needed
include("cmake/Utils.cmake")

# different compile options
option(DISABLE_QT_DEBUG "Disable Qt Debug Messages" OFF)

# load paths from the user file if exists 
if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeUser.cmake)
	include(${CMAKE_SOURCE_DIR}/CMakeUser.cmake)
endif()

PIE_CHECK_COMPILER()

if(CMAKE_COMPILER_IS_GNUCXX)
	# Codecov
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wno-attributes -Wno-unknown-pragmas -pthread -g -O0 -fprofile-arcs -ftest-coverage")
endif()

# find Qt
PIE_FIND_QT()

# find OpenCV
PIE_FIND_OPENCV()

if (DISABLE_QT_DEBUG)
	message (STATUS "disabling qt debug messages")
	add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

if(MSVC)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Win.cmake)
elseif(UNIX)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Unix.cmake)
else()
	message(STATUS "build system unkown ... fallback to unix")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Unix.cmake)
endif()

file(GLOB MAIN_SOURCES "src/*.cpp")
file(GLOB MAIN_HEADERS "src/*.h")

# unit tests
file(GLOB TEST_SOURCES "src/UnitTests/*.cpp")
file(GLOB TEST_HEADERS "src/UnitTests/*.h")

# core
file(GLOB CORE_SOURCES "src/Core/*.cpp")
file(GLOB CORE_HEADERS "src/Core/*.h")

# external headers
file(GLOB TSNE_SOURCES "src/3rdparty/tsne/*.cpp")
file(GLOB TSNE_HEADERS "src/3rdparty/tsne/*.h")

# gather information for building
include_directories (
	${CMAKE_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty
	${CMAKE_CURRENT_SOURCE_DIR}/src/Core
	)

set(	PIE_RESOURCES
		src/pie.qrc
	)

QT5_ADD_RESOURCES(PIE_RCC ${PIE_RESOURCES})

# TODO: uncomment if we ever need translations
# file(GLOB PIE_TRANSLATIONS "translations/*.ts")
# QT5_ADD_TRANSLATION(PIE_QM ${PIE_TRANSLATIONS})

if(UNIX)

	# enable CTest
	include(CTest)
	enable_testing()

endif()

# create build targets
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildTargets.cmake)

# Codecov
if(CMAKE_COMPILER_IS_GNUCXX)
	include("cmake/CodeCoverage.cmake")
    setup_target_for_coverage(${PROJECT_NAME}_coverage ${PIE_BINARY_NAME} coverage)
endif()

