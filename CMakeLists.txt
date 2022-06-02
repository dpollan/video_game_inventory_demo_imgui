cmake_minimum_required(VERSION 3.0)
project(runMe CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(imgui CONFIG)
find_package(glfw3 CONFIG)
find_package(glew CONFIG)

add_executable( runMe
                main.cpp
                src/Character.cpp
                src/Character.h
                src/Item.h
                src/Item.cpp
                opengl_shader.cpp
                file_manager.cpp
                opengl_shader.h
                file_manager.h
                bindings/imgui_impl_glfw.cpp
                bindings/imgui_impl_opengl3.cpp
                bindings/imgui_impl_glfw.h
                bindings/imgui_impl_opengl3.h
                )

add_custom_command(TARGET runMe
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.vs ${PROJECT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.fs ${PROJECT_BINARY_DIR}
)
#"C:\\code\\examples\\libraries\\dear-imgui\\basic\\""

target_compile_definitions(runMe PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(runMe imgui::imgui GLEW::glew_s glfw::glfw)
target_include_directories(runMe PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src)