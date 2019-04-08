cmake_minimum_required(VERSION 3.10)
project(imgui C CXX)

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLEW)
set(imgui_HEADERS
    imgui/imconfig.h imgui/imgui.h imgui/imgui_internal.h imgui/imstb_rectpack.h
    imgui/imstb_textedit.h imgui/imstb_truetype.h
    imgui/examples/imgui_impl_glfw.h imgui/examples/imgui_impl_opengl3.h)

set(imgui_SOURCES
    imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp
    imgui/examples/imgui_impl_glfw.cpp imgui/examples/imgui_impl_opengl3.cpp)

add_library(imgui STATIC ${imgui_HEADERS} ${imgui_SOURCES})
target_include_directories(imgui PRIVATE ${CMAKE_SOURCE_DIR}/../include ${CMAKE_SOURCE_DIR}/imgui ${CMAKE_SOURCE_DIR}/imgui/examples)
install(TARGETS imgui ARCHIVE DESTINATION lib)
install(FILES ${imgui_HEADERS} DESTINATION include)
