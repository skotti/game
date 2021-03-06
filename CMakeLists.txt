cmake_minimum_required(VERSION 2.6)
project(game)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(cube_game 
	src/main.cpp
	src/debug.cpp
	src/engine.cpp
	src/input_notifier.cpp
	src/printf.cpp
	src/window.cpp
	src/shader.cpp
	src/mouse_notifier.cpp
	src/maze_generator.cpp
	src/game_logic.cpp
	src/player_logic.cpp
	src/game_object.cpp
	src/game_graphic.cpp
	src/utils.cpp
	src/menu.cpp
	src/random.cpp
	src/shadow.cpp
	src/skybox.cpp
	src/dynamic_enemy_logic.cpp

	src/audio/player.cpp

	src/models/object.cpp
	src/models/material.cpp
	src/models/texture.cpp
	)

target_include_directories(cube_game PUBLIC src)
target_compile_options(cube_game PUBLIC -g3 -O0 -std=c++14)

configure_file(shaders/light_fragment.frag shaders/light_fragment.frag COPYONLY)
configure_file(shaders/light_vertex.vert shaders/light_vertex.vert COPYONLY)
configure_file(shaders/depth_fragment.frag shaders/depth_fragment.frag COPYONLY)
configure_file(shaders/depth_vertex.vert shaders/depth_vertex.vert COPYONLY)
configure_file(shaders/material_fragment.frag shaders/material_fragment.frag COPYONLY)
configure_file(shaders/material_vertex.vert shaders/material_vertex.vert COPYONLY)
configure_file(shaders/texture_fragment.frag shaders/texture_fragment.frag COPYONLY)
configure_file(shaders/texture_vertex.vert shaders/texture_vertex.vert COPYONLY)

configure_file(shaders/text_fragment.frag shaders/text_fragment.frag COPYONLY)
configure_file(shaders/text_vertex.vert shaders/text_vertex.vert COPYONLY)

configure_file(fonts/DejaVuSansMono.ttf fonts/DejaVuSansMono.ttf COPYONLY)

configure_file(shaders/skybox_fragment.frag shaders/skybox_fragment.frag COPYONLY)
configure_file(shaders/skybox_vertex.vert shaders/skybox_vertex.vert COPYONLY)

configure_file(shaders/test_depth.frag shaders/test_depth.frag COPYONLY)
configure_file(shaders/test_depth.vert shaders/test_depth.vert COPYONLY)

configure_file(shaders/cascade_depth.frag shaders/cascade_depth.frag COPYONLY)
configure_file(shaders/cascade_depth.vert shaders/cascade_depth.vert COPYONLY)
configure_file(shaders/cascade_shadow.frag shaders/cascade_shadow.frag COPYONLY)
configure_file(shaders/cascade_shadow.vert shaders/cascade_shadow.vert COPYONLY)

configure_file(textures/envmap_violentdays/violentdays_rt.tga textures/envmap_violentdays/violentdays_rt.tga COPYONLY)
configure_file(textures/envmap_violentdays/violentdays_lf.tga textures/envmap_violentdays/violentdays_lf.tga COPYONLY)
configure_file(textures/envmap_violentdays/violentdays_up.tga textures/envmap_violentdays/violentdays_up.tga COPYONLY)
configure_file(textures/envmap_violentdays/violentdays_dn.tga textures/envmap_violentdays/violentdays_dn.tga COPYONLY)
configure_file(textures/envmap_violentdays/violentdays_bk.tga textures/envmap_violentdays/violentdays_bk.tga COPYONLY)
configure_file(textures/envmap_violentdays/violentdays_ft.tga textures/envmap_violentdays/violentdays_ft.tga COPYONLY)


configure_file(models/model_cube.obj models/model_cube.obj COPYONLY)
#configure_file(models/model_cube.mtl models/model_cube.mtl COPYONLY)

configure_file(models/cube.obj models/cube.obj COPYONLY)
configure_file(models/cube.mtl models/cube.mtl COPYONLY)

configure_file(models/eyeball.obj models/eyeball.obj COPYONLY)
configure_file(models/eyeball.mtl models/eyeball.mtl COPYONLY)

configure_file(models/vm.obj models/vm.obj COPYONLY)
configure_file(models/vm.mtl models/vm.mtl COPYONLY)
configure_file(textures/Virtual-Machines-Icon.png textures/Virtual-Machines-Icon.png COPYONLY)

configure_file(models/wall.obj models/wall.obj COPYONLY)
configure_file(models/wall.mtl models/wall.mtl COPYONLY)

configure_file(models/cube_textured.obj models/cube_textured.obj COPYONLY)
configure_file(models/cube_textured.mtl models/cube_textured.mtl COPYONLY)
configure_file(textures/grass4k.jpg textures/grass4k.jpg COPYONLY)
configure_file(textures/wildtextures_stone-bricks-wall.jpg textures/wildtextures_stone-bricks-wall.jpg COPYONLY)


configure_file(models/start_cube.obj models/start_cube.obj COPYONLY)
configure_file(models/finish_cube.obj models/finish_cube.obj COPYONLY)
configure_file(models/cube1.obj models/cube1.obj COPYONLY)
configure_file(models/cube2.obj models/cube2.obj COPYONLY)
configure_file(models/cube3.obj models/cube3.obj COPYONLY)
configure_file(models/cube4.obj models/cube4.obj COPYONLY)

configure_file(models/start_cube.mtl models/start_cube.mtl COPYONLY)
configure_file(models/finish_cube.mtl models/finish_cube.mtl COPYONLY)
configure_file(models/cube1.mtl models/cube1.mtl COPYONLY)
configure_file(models/cube2.mtl models/cube2.mtl COPYONLY)
configure_file(models/cube3.mtl models/cube3.mtl COPYONLY)
configure_file(models/cube4.mtl models/cube4.mtl COPYONLY)

configure_file(textures/start_cube.tga textures/start_cube.tga COPYONLY)
configure_file(textures/finish_cube.tga textures/finish_cube.tga COPYONLY)
configure_file(textures/cube1.tga textures/cube1.tga COPYONLY)
configure_file(textures/cube2.tga textures/cube2.tga COPYONLY)
configure_file(textures/cube3.tga textures/cube3.tga COPYONLY)
configure_file(textures/cube4.tga textures/cube4.tga COPYONLY)

configure_file(textures/start_cube_top.tga textures/start_cube_top.tga COPYONLY)
configure_file(textures/finish_cube_top.tga textures/finish_cube_top.tga COPYONLY)
configure_file(textures/cube1_top.tga textures/cube1_top.tga COPYONLY)
configure_file(textures/cube2_top.tga textures/cube2_top.tga COPYONLY)
configure_file(textures/cube3_top.tga textures/cube3_top.tga COPYONLY)
configure_file(textures/cube4_top.tga textures/cube4_top.tga COPYONLY)

configure_file(audio/menu_hover.wav audio/menu_hover.wav COPYONLY)
configure_file(audio/menu_select.wav audio/menu_select.wav COPYONLY)
configure_file(audio/ambient_background.ogg audio/ambient_background.ogg COPYONLY)
configure_file(audio/win.ogg audio/win.ogg COPYONLY)

configure_file(models/plane.obj models/plane.obj COPYONLY)
configure_file(models/plane.mtl models/plane.mtl COPYONLY)


configure_file(textures/Eye_D.jpg textures/Eye_D.jpg COPYONLY)
configure_file(textures/Eye_N.jpg textures/Eye_N.jpg COPYONLY)
configure_file(textures/REF_1.jpg textures/REF_1.jpg COPYONLY)

configure_file(textures/E-45_glass_nor_.jpg textures/E-45_glass_nor_.jpg COPYONLY)
configure_file(textures/E-45_col.jpg textures/E-45_col.jpg COPYONLY)
configure_file(models/E_45_Aircraft_obj.obj models/E_45_Aircraft_obj.obj COPYONLY)
configure_file(models/E_45_Aircraft_obj.mtl models/E_45_Aircraft_obj.mtl COPYONLY)

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

find_package(assimp REQUIRED)

target_include_directories(cube_game PUBLIC ${assimp_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${assimp_LIBRARIES})

find_package(Freetype REQUIRED)

target_include_directories(cube_game PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(cube_game PUBLIC ${FREETYPE_LIBRARIES})

find_package(OpenAL REQUIRED)
target_include_directories(cube_game PUBLIC ${OPENAL_INCLUDE_DIR})
target_link_libraries(cube_game PUBLIC ${OPENAL_LIBRARY})
message(STATUS "OpenAL includes: " ${OPENAL_INCLUDE_DIR})
message(STATUS "OpenAL includes: " ${OPENAL_LIBRARY})
if(NOT OPENAL_FOUND)
	message(FATAL_ERROR "OpenAL is not found")
endif(NOT OPENAL_FOUND)
