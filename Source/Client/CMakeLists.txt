file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)
add_pch(Precompiled.h Precompiled.cpp ${SOURCES})
add_library(Client SHARED ${SOURCES} ${HEADERS})
add_definitions(-DBUILDING_CLIENT)
include_directories(${STB_INCLUDEDIR})
target_link_libraries(Client
	${SDL2_LIBS}
	${GLEW_LIBS}
	${ENET_LIBS}
	${OPENGL_gl_LIBRARY}
	${OPENAL_LIBS}
	${TMXPARSER_LIBS}
	${BOX2D_LIBS})
target_link_libraries(Client Core)
target_link_libraries(Client Physics)