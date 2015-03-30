file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)
#add_executable(WildDust ${WIN32_NOCONSOLE} ${SOURCES} ${HEADERS})
add_executable(WildDust MACOSX_BUNDLE ${WIN32_NOCONSOLE} ${SOURCES} ${HEADERS})
target_link_libraries(WildDust Network Renderer Client Server Game Core Audio)
if (APPLE)
	add_custom_command(
	TARGET WildDust
	POST_BUILD
	COMMAND rsync
	ARGS -r
	  "${CONTENT_DATA_PATH}"
	  "${CMAKE_SOURCE_DIR}/Binaries/WildDust.app/Contents/"
	COMMENT "Copying Bundle Stuff")

	add_custom_command(
	TARGET WildDust
	POST_BUILD
	COMMAND rsync
	ARGS -r
	  "${CONFIG_DATA_PATH}"
	  "${CMAKE_SOURCE_DIR}/Binaries/WildDust.app/Contents/MacOS"
	COMMENT "Copying Bundle Stuff")
endif()