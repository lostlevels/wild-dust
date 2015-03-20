include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${SDL2_INCLUDEDIR}
	${GLEW_INCLUDEDIR}
	${ENET_INCLUDEDIR}
	${GLM_INCLUDEDIR}
	${BOX2D_INCLUDEDIR})
	
link_directories(
	${SDL2_LIBDIR}
	${GLEW_LIBDIR}
	${ENET_LIBDIR})

add_subdirectory(Core)
add_subdirectory(Client)
add_subdirectory(Server)
add_subdirectory(Game)
