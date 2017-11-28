# If you want to use prefix paths with cmake, copy and rename this file to CMakeUser.cmake
# Do not add this file to GIT!

# set your preferred library paths
IF (CMAKE_CL_64)
	SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Qt/qt-everywhere-opensource-src-5.9.1-x64/qtbase/bin/")
	SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/coding/3rd-party/OpenCV/build2017-x64")

ELSE ()
	SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Qt/Qt5.9/5.9.2/msvc2015/bin")
	SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/coding/3rd-party/OpenCV/build2017-x86")
ENDIF ()


