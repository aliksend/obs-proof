project(obs-cheadless)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(_lib_suffix 64)
else()
	set(_lib_suffix 32)
endif()

find_package(FFmpeg REQUIRED COMPONENTS avcodec avutil avformat)

include_directories(${FFMPEG_INCLUDE_DIRS})
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/libobs")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/deps/libff")

find_package(Libcurl REQUIRED)
include_directories(${LIBCURL_INCLUDE_DIRS})
add_definitions(${LIBCURL_DEFINITIONS})

if(WIN32)
	include_directories(${OBS_JANSSON_INCLUDE_DIRS})
	include_directories(${BLAKE2_INCLUDE_DIR})

	# set(obs_PLATFORM_SOURCES
	# 	platform-windows.cpp
	# 	win-update/update-window.cpp
	# 	win-update/win-update.cpp
	# 	win-update/win-update-helpers.cpp
	# 	obs.rc)
	# set(obs_PLATFORM_HEADERS
	# 	win-update/update-window.hpp
	# 	win-update/win-update.hpp
	# 	win-update/win-update-helpers.hpp)
	set(obs_PLATFORM_LIBRARIES
		crypt32
		blake2
		${OBS_JANSSON_IMPORT})

	if(ENABLE_WIN_UPDATER)
		add_definitions(-DENABLE_WIN_UPDATER)
	endif()
elseif(APPLE)
	# set(obs_PLATFORM_SOURCES
	# 	platform-osx.mm)

	find_package(AppKit REQUIRED)
	set(obs_PLATFORM_LIBRARIES ${APPKIT_LIBRARIES})

	add_definitions(-fobjc-arc)

	# option(ENABLE_SPARKLE_UPDATER "Enables updates via the Sparkle framework (don't forget to update the Info.plist for your .app)" OFF)
	# if(ENABLE_SPARKLE_UPDATER)
	# 	find_library(SPARKLE Sparkle)
	# 	include_directories(${SPARKLE})
	# 	set(obs_PLATFORM_SOURCES
	# 		${obs_PLATFORM_SOURCES}
	# 		sparkle-updater.mm)
	# 	set(obs_PLATFORM_LIBRARIES
	# 		${obs_PLATFORM_LIBRARIES}
	# 		${SPARKLE})
	# 	add_definitions(-DUPDATE_SPARKLE=1)
	# endif()

elseif(UNIX)
        # find_package(Qt5X11Extras REQUIRED)

	# set(obs_PLATFORM_SOURCES
	# 	platform-x11.cpp)

        set(obs_PLATFORM_LIBRARIES
                # Qt5::X11Extras
								X11
							)
endif()

set(obs_libffutil_SOURCES
	../deps/libff/libff/ff-util.c
	)
set(obs_libffutil_HEADERS
	../deps/libff/libff/ff-util.h
	)

if(MSVC)
	set_source_files_properties(
		../deps/libff/libff/ff-util.c
		PROPERTIES COMPILE_FLAGS -Dinline=__inline
		)
endif()

set(obs_SOURCES
	# ${obs_PLATFORM_SOURCES}
	${obs_libffutil_SOURCES}
	main.c)

set(obs_HEADERS
	# ${obs_PLATFORM_HEADERS}
	${obs_libffutil_HEADERS})

add_executable(obs-cheadless WIN32
	${obs_SOURCES}
	${obs_HEADERS})

if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_output_suffix "64")
	else()
		set(_output_suffix "32")
	endif()

	set_target_properties(obs-cheadless
		PROPERTIES
			OUTPUT_NAME "obs${_output_suffix}")
endif()

target_link_libraries(obs-cheadless
	libobs
	${FFMPEG_LIBRARIES}
	${LIBCURL_LIBRARIES}
	${obs_PLATFORM_LIBRARIES})

if (APPLE)
	# target_link_libraries(obs-cheadless
	# 		Qt5::MacExtras)
	set_target_properties(obs-cheadless PROPERTIES LINK_FLAGS "-pagezero_size 10000 -image_base 100000000")
	set_property(
		TARGET obs-cheadless
		APPEND
		PROPERTY INSTALL_RPATH
		"/usr/local/Cellar/python3/3.6.4_2/Frameworks/Python.framework/Versions/3.6/lib/"
		"/Library/Frameworks/Python.framework/Versions/3.6/lib/"
		"/opt/local/Library/Frameworks/Python.framework/Versions/3.6/lib/"
	)
endif()

define_graphic_modules(obs-cheadless)

install_obs_core(obs-cheadless)
install_obs_data(obs-cheadless data obs-studio)

if (UNIX AND UNIX_STRUCTURE AND NOT APPLE)
	install(FILES dist/obs.desktop DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications)
	install(FILES forms/images/obs.png
		DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/hicolor/256x256/apps)
endif()
