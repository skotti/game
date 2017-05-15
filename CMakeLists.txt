cmake_minimum_required(VERSION 2.6)
project(game)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_executable(cube_game 
	main.cpp
	debug.cpp
	engine.cpp
	input_notifier.cpp
	model.cpp
	printf.cpp
	window.cpp
	shader.cpp
	mouse_notifier.cpp
	texture.cpp
	maze_generator.cpp
	game_logic.cpp
	player_logic.cpp
	game_object.cpp
	game_graphic.cpp
	utils.cpp
	shadow.cpp
	skybox.cpp
	)

target_compile_options(cube_game PUBLIC -g -std=c++14)

configure_file(shaders/light_fragment.frag shaders/light_fragment.frag COPYONLY)
configure_file(shaders/light_vertex.vert shaders/light_vertex.vert COPYONLY)
configure_file(shaders/depth_fragment.frag shaders/depth_fragment.frag COPYONLY)
configure_file(shaders/depth_vertex.vert shaders/depth_vertex.vert COPYONLY)
configure_file(shaders/material_fragment.frag shaders/material_fragment.frag COPYONLY)
configure_file(shaders/material_vertex.vert shaders/material_vertex.vert COPYONLY)
configure_file(shaders/texture_fragment.frag shaders/texture_fragment.frag COPYONLY)
configure_file(shaders/texture_vertex.vert shaders/texture_vertex.vert COPYONLY)
configure_file(shaders/skybox_fragment.frag shaders/skybox_fragment.frag COPYONLY)
configure_file(shaders/skybox_vertex.vert shaders/skybox_vertex.vert COPYONLY)

configure_file(shaders/test_depth.frag shaders/test_depth.frag COPYONLY)
configure_file(shaders/test_depth.vert shaders/test_depth.vert COPYONLY)

configure_file(shaders/cascade_depth.frag shaders/cascade_depth.frag COPYONLY)
configure_file(shaders/cascade_depth.vert shaders/cascade_depth.vert COPYONLY)
configure_file(shaders/cascade_shadow.frag shaders/cascade_shadow.frag COPYONLY)
configure_file(shaders/cascade_shadow.vert shaders/cascade_shadow.vert COPYONLY)

configure_file(textures/right.jpg textures/right.jpg COPYONLY)
configure_file(textures/left.jpg textures/left.jpg COPYONLY)
configure_file(textures/top.jpg textures/top.jpg COPYONLY)
configure_file(textures/bottom.jpg textures/bottom.jpg COPYONLY)
configure_file(textures/back.jpg textures/back.jpg COPYONLY)
configure_file(textures/front.jpg textures/front.jpg COPYONLY)


configure_file(models/modelCube.obj models/modelCube.obj COPYONLY)
configure_file(models/suzanna.obj models/suzanna.obj COPYONLY)

find_package(OpenGL REQUIRED)

target_include_directories(cube_game PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)

target_include_directories(cube_game PUBLIC ${GLEW_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${GLEW_LIBRARIES})

find_package(GLFW REQUIRED)

target_include_directories(cube_game PUBLIC ${GLFW_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${GLFW_LIBRARIES})

find_package(SOIL REQUIRED)

target_include_directories(cube_game PUBLIC ${SOIL_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${SOIL_LIBRARIES})

find_package(Yaml REQUIRED)

target_include_directories(cube_game PUBLIC ${YAML_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${YAML_LIBRARIES})

find_package(assimp REQUIRED)

target_include_directories(cube_game PUBLIC ${assimp_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${assimp_LIBRARIES})

find_package(OpenAL REQUIRED)

target_include_directories(cube_game PUBLIC ${OPENAL_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${OPENAL_LIBRARIES})

find_package(ALUT REQUIRED)

target_include_directories(cube_game PUBLIC ${ALUT_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${ALUT_LIBRARIES})
