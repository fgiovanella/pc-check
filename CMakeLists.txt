cmake_minimum_required(VERSION 3.15)
project(MeuMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.89.9
)
FetchContent_MakeAvailable(imgui)

find_package(OpenGL REQUIRED)

add_executable(MeuMonitor 
    src/main.cpp
    src/backend.cpp

    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(MeuMonitor PUBLIC 
    glfw
    ${OPENGL_LIBRARIES}
)

target_include_directories(MeuMonitor PUBLIC 
    src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)