cmake_minimum_required(VERSION 3.15)
project(MeuMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Baixa e configura o GLFW (para criar a janela)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# Baixa e configura o Dear ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.89.9
)
FetchContent_MakeAvailable(imgui)

# Encontra o OpenGL (necessário para renderizar)
find_package(OpenGL REQUIRED)

# --- A SEÇÃO CORRIGIDA ---

# Cria seu programa executável
add_executable(MeuMonitor 
    src/main.cpp
    src/backend.cpp

    # Adiciona os arquivos-fonte PRINCIPAIS do ImGui
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    # Adiciona os arquivos-fonte de BACKEND do ImGui
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Linka seu programa SOMENTE com as bibliotecas EXTERNAS
target_link_libraries(MeuMonitor PUBLIC 
    glfw   # A biblioteca de janela
    ${OPENGL_LIBRARIES} # O OpenGL
)

# Adiciona os includes necessários para compilar
target_include_directories(MeuMonitor PUBLIC 
    src # Permite que main.cpp encontre backend.h
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)