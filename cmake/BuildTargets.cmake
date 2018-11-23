
# create the targets
set(PIE_BINARY_NAME ${PROJECT_NAME})		# binary
set(PIE_DLL_CORE_NAME ${PROJECT_NAME}Core)	# library

if (MSVC)
	set(PIE_RC src/pie.rc) #add resource file when compiling with MSVC 
	set(VERSION_LIB Version.lib)
	set(LIBRARY_DIR ${CMAKE_BINARY_DIR}/libs/) #add libs directory to build dir
	set(PIE_LIB_CORE_NAME optimized ${PIE_DLL_CORE_NAME}.lib debug ${PIE_DLL_CORE_NAME}d.lib)
else ()
	set(PIE_LIB_CORE_NAME ${PIE_DLL_CORE_NAME})	# forward declarem
endif()

#binary
link_directories(${OpenCV_LIBRARY_DIRS} ${LIBRARY_DIR})
set(CHANGLOG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/changelog.txt)
add_executable(${PIE_BINARY_NAME} WIN32  MACOSX_BUNDLE ${MAIN_SOURCES} ${MAIN_HEADERS} ${PIE_TRANSLATIONS} ${PIE_RC} ${PIE_QRC} ${CHANGLOG_FILE}) #changelog is added here, so that i appears in visual studio
set_source_files_properties(${CHANGLOG_FILE} PROPERTIES HEADER_FILE_ONLY TRUE) # define that changelog should not be compiled

target_link_libraries(${PIE_BINARY_NAME} ${PIE_LIB_CORE_NAME} ${OpenCV_LIBS} ${VERSION_LIB}) 
set_target_properties(${PIE_BINARY_NAME} PROPERTIES COMPILE_FLAGS "-DNOMINMAX")

# add core
add_library(
	${PIE_DLL_CORE_NAME} SHARED 
	${CORE_SOURCES} ${CORE_HEADERS} 
	${TSNE_HEADERS} ${TSNE_SOURCES} 		# t-SNE
	${PIE_RC}
	)
target_link_libraries(${PIE_DLL_CORE_NAME} ${VERSION_LIB} ${OpenCV_LIBS}) 

add_dependencies(${PIE_BINARY_NAME} ${PIE_DLL_CORE_NAME}) 

target_include_directories(${PIE_BINARY_NAME} 		PRIVATE ${OpenCV_INCLUDE_DIRS})
target_include_directories(${PIE_DLL_CORE_NAME} 	PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PIE_BINARY_NAME} Qt5::Widgets Qt5::Gui Qt5::Network Qt5::Concurrent Qt5::Svg)
target_link_libraries(${PIE_DLL_CORE_NAME} Qt5::Widgets Qt5::Gui Qt5::Network Qt5::Concurrent Qt5::Svg)

if (MSVC)
	target_link_libraries(${PIE_BINARY_NAME} Qt5::WinExtras)
	target_link_libraries(${PIE_DLL_CORE_NAME} Qt5::WinExtras)
endif()

# core flags
set_target_properties(${PIE_DLL_CORE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/libs/$<CONFIGURATION>)
set_target_properties(${PIE_DLL_CORE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/libs/$<CONFIGURATION>)
set_target_properties(${PIE_DLL_CORE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/libs/$<CONFIGURATION>)
set_target_properties(${PIE_DLL_CORE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/libs/$<CONFIGURATION>)

set_target_properties(${PIE_DLL_CORE_NAME} PROPERTIES COMPILE_FLAGS "-DDLL_CORE_EXPORT -DNOMINMAX")
set_target_properties(${PIE_DLL_CORE_NAME} PROPERTIES DEBUG_OUTPUT_NAME ${PIE_DLL_CORE_NAME}d)
set_target_properties(${PIE_DLL_CORE_NAME} PROPERTIES RELEASE_OUTPUT_NAME ${PIE_DLL_CORE_NAME})

# make RelWithDebInfo link against release instead of debug opencv dlls
set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_MINSIZEREL RELEASE)

# create settings file for portable version while working
if(NOT EXISTS ${CMAKE_BINARY_DIR}/Release/rdf-settings.nfo)
	file(WRITE ${CMAKE_BINARY_DIR}/Release/rdf-settings.nfo "")
endif()
if(NOT EXISTS ${CMAKE_BINARY_DIR}/RelWithDebInfo/rdf-settings.nfo)
	file(WRITE ${CMAKE_BINARY_DIR}/RelWithDebInfo/rdf-settings.nfo "")
endif()
if(NOT EXISTS ${CMAKE_BINARY_DIR}/Debug/rdf-settings.nfo)
	file(WRITE ${CMAKE_BINARY_DIR}/Debug/rdf-settings.nfo "")
endif()

if (MSVC)
	# set properties for Visual Studio Projects
	# add_definitions(/Zc:wchar_t-)
	set(CMAKE_CXX_FLAGS_DEBUG "/W4 ${CMAKE_CXX_FLAGS_DEBUG}")
	set(CMAKE_CXX_FLAGS_RELEASE "/W4 /O2 ${CMAKE_CXX_FLAGS_RELEASE}")
	
	source_group("Generated Files" FILES ${PIE_RC} ${PIE_QM} ${PIE_AUTOMOC})
	source_group("graphcut" FILES ${GC_HEADERS} ${GC_SOURCES})
	source_group("LSD" FILES ${LSD_HEADERS} ${LSD_SOURCES})
	source_group("maxclique" FILES ${MC_HEADERS} ${MC_SOURCES})
	source_group("Changelog" FILES ${CHANGLOG_FILE})

	# set as console project 
	set_target_properties(${PIE_BINARY_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(${PIE_BINARY_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
	set_target_properties(${PIE_BINARY_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	
endif ()

file(GLOB PIE_AUTOMOC "${CMAKE_BINARY_DIR}/*_automoc.cpp")

# generate configuration file
if (PIE_DLL_CORE_NAME)
	get_property(CORE_DEBUG_NAME TARGET ${PIE_DLL_CORE_NAME} PROPERTY DEBUG_OUTPUT_NAME)
	get_property(CORE_RELEASE_NAME TARGET ${PIE_DLL_CORE_NAME} PROPERTY RELEASE_OUTPUT_NAME)
endif()

set(PIE_OPENCV_BINARIES ${PIE_OPENCV_BINARIES})	# just to show it goes into cmake.in
set(PIE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PIE_BUILD_DIRECTORY ${CMAKE_BINARY_DIR})
set(
	PIE_INCLUDE_DIRECTORY 
	${CMAKE_CURRENT_SOURCE_DIR}/src 
	${CMAKE_CURRENT_SOURCE_DIR}/src/Core 
	${CMAKE_CURRENT_SOURCE_DIR}/src/Module 
	${CMAKE_BINARY_DIR})

if (MSVC)
	set(
		PIE_BINARIES 
		${CMAKE_BINARY_DIR}/Debug/${CORE_DEBUG_NAME}.dll 
		${CMAKE_BINARY_DIR}/Release/${CORE_RELEASE_NAME}.dll
		)
	set(PIE_CORE_LIB optimized ${CMAKE_BINARY_DIR}/libs/Release/${CORE_RELEASE_NAME}.lib debug  ${CMAKE_BINARY_DIR}/libs/Debug/${CORE_DEBUG_NAME}.lib)

else ()
	set(PIE_BINARIES 
		${CMAKE_CURRENT_BINARY_DIR}/lib${PIE_DLL_CORE_NAME}.so 
		)
endif()

set(PIE_LIBS ${PIE_CORE_LIB})

configure_file(${PIE_SOURCE_DIR}/PIE.cmake.in ${CMAKE_BINARY_DIR}/PIE.cmake)

# tests
# add_test(NAME tSNE COMMAND ${PIE_BINARY_NAME} "--tSNE")

#package 
if (UNIX)

	# install
	install(TARGETS ${PIE_BINARY_NAME} ${PIE_DLL_MODULE_NAME} ${PIE_DLL_CORE_NAME} DESTINATION bin LIBRARY DESTINATION lib${LIB_SUFFIX})

	# "make dist" target
	string(TOLOWER ${PROJECT_NAME} CPACK_PACKAGE_NAME)
	set(CPACK_PACKAGE_VERSION "${PIE_FRAMEWORK_VERSION}")
	set(CPACK_SOURCE_GENERATOR "TBZ2")
	set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
	set(CPACK_IGNORE_FILES "/CVS/;/\\\\.svn/;/\\\\.git/;\\\\.swp$;\\\\.#;/#;\\\\.tar.gz$;/CMakeFiles/;CMakeCache.txt;refresh-copyright-and-license.pl;build;release;")
	set(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})
	include(CPack)
	# simulate autotools' "make dist"

	if(DEFINED GLOBAL_READ_BUILD)
		if(NOT ${GLOBAL_READ_BUILD}) # cannot be incooperated into if one line above
			add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
		endif()
	endif()
elseif (MSVC)
	### DependencyCollector
	set(DC_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DependencyCollector.py)
	set(DC_CONFIG ${CMAKE_BINARY_DIR}/DependencyCollector.ini)

	# CMAKE_MAKE_PROGRAM works for VS 2017 too
	get_filename_component(VS_PATH ${CMAKE_MAKE_PROGRAM} PATH)
	if(CMAKE_CL_64)
		set(VS_PATH "${VS_PATH}/../../../Common7/IDE/Remote Debugger/x64")
	else()
		set(VS_PATH "${VS_PATH}/../../Common7/IDE/Remote Debugger/x86")
	endif()
	set(DC_PATHS_RELEASE ${OpenCV_DIR}/bin/Release ${QT_QMAKE_PATH} ${VS_PATH})
	set(DC_PATHS_DEBUG ${OpenCV_DIR}/bin/Debug ${QT_QMAKE_PATH} ${VS_PATH})

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DependencyCollector.config.cmake.in ${DC_CONFIG})

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND python ${DC_SCRIPT} --infile $<TARGET_FILE:${PROJECT_NAME}> --configfile ${DC_CONFIG} --configuration $<CONFIGURATION>)
endif()

