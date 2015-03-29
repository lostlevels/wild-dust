file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)
add_executable(WildDust ${WIN32_NOCONSOLE} ${SOURCES} ${HEADERS})
target_link_libraries(WildDust Network Renderer Client Server Game Core Audio)