# If you want to use prefix paths with cmake, copy and rename this file to CMakeUser.cmake
# Do not add this file to GIT!

# set your preferred OpenCV Library path
IF (CMAKE_CL_64)
	message(STATUS "using x64 architecture...")
	SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Qt/Qt5.11.1-x64/bin")
	SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/coding/3rd-party/opencv/build2017-x64/")
ELSE()
	message(STATUS "using x86 architecture...")
	SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Qt/Qt5.11.0/5.11.0/msvc2015/bin")
	SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/coding/3rd-party/opencv/build2017-x86/")
ENDIF()
