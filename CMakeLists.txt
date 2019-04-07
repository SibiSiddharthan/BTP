cmake_minimum_required(VERSION 3.13)
project(MESH C CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds not allowed. Please make a new directory")
endif()

set(mesh_HEADERS 
	src/2d.h src/3d.h src/dxfreader.h src/position.h src/geometry.h src/gl_abstraction.h
	src/mesh2d.h src/model2d.h src/mesh3d.h src/model3d.h src/stl_reader.h src/window.h
	src/linear_algebra.h src/preprocess.h)

set(mesh_SOURCES
	src/main.cpp src/model2d.cpp src/model3d.cpp src/geometry.cpp src/stl_reader.cpp
	src/dxfreader.cpp src/linear_algebra.cpp src/window.cpp
	src/mesh2d_debug.cpp src/mesh2d_display.cpp src/mesh2d_private src/mesh2d_public.cpp
	src/mesh3d_display.cpp src/mesh3d_private )#src/mesh3d_public.cpp)

if(CMAKE_BUILD_TYPE STREQUAL Release)
	if(NOT MSVC)
		set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
		set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" )
	endif()
endif()

add_executable(mesh ${mesh_HEADERS} ${mesh_SOURCES})
set_property(TARGET mesh PROPERTY CXX_STANDARD 17)
target_link_libraries(mesh glew32 glfw3 imgui opengl32)
target_compile_definitions(mesh PRIVATE GLEW_STATIC)
target_include_directories(mesh PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(mesh PUBLIC -fdiagnostics-color=always)
target_link_directories(mesh PRIVATE ${CMAKE_SOURCE_DIR}/lib)
add_custom_command(TARGET mesh POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/mesh.exe ${CMAKE_SOURCE_DIR})
