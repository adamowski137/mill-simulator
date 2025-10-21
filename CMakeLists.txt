cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mill)

add_executable(${PROJECT_NAME} 
  main.cpp
  core/App.cpp
  core/Camera.cpp
  core/Window.cpp
  core/Scene.cpp
  core/Renderer.cpp
  core/Shader.cpp
  core/Parser.cpp
  core/Simulator.cpp
  core/ComputeShader.cpp
  core/models/Entity.cpp
  core/models/Ground.cpp
  core/models/Blade.cpp
  core/models/Block.cpp
  core/models/Path.cpp
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
add_compile_definitions(_USE_MATH_DEFINES)

# Path setup
set(SHADER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_BINARY_DIR ${CMAKE_BINARY_DIR}/shaders)

# Create copy rule
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADER_SOURCE_DIR} ${SHADER_BINARY_DIR}
    COMMENT "Copying shaders to build directory..."
)

# Ensure shaders are copied before your executable runs
add_dependencies(${PROJECT_NAME} copy_shaders)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/math137)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glew-cmake)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/ImGuiFileDialog)


find_package(OpenGL REQUIRED)

add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC 
  external/imgui 
  external/imgui/backends
)

target_include_directories(math137 PUBLIC external/math137/include)
target_link_libraries(imgui PRIVATE glfw OpenGL::GL)
target_link_libraries(ImGuiFileDialog PRIVATE imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE 
  imgui 
  math137 
  libglew_static 
  OpenGL::GL 
  glfw 
  ImGuiFileDialog
)
