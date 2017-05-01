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
	object_view.cpp
	shader.cpp
	camera.cpp
	mouse_notifier.cpp
	texture.cpp
	)

target_compile_options(cube_game PUBLIC -g -std=c++14)

configure_file(config.yaml config.yaml COPYONLY)
configure_file(models/modelCube.obj models/modelCube.obj COPYONLY)
configure_file(models/suzanna.obj models/suzanna.obj COPYONLY)
configure_file(textures/dragon.png textures/dragon.png COPYONLY)
configure_file(shaders/fragment.frag shaders/fragment.frag COPYONLY)
configure_file(shaders/vertex.vert shaders/vertex.vert COPYONLY)

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
