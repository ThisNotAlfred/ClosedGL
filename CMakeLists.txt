cmake_minimum_required(VERSION 3.28)
project(ClosedGl CXX)

# c++23 and c23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

# forcing everyting to be static
set(BUILD_SHARED_LIBS OFF)

# -----------------------------

# ClosedGl
set(ClosedGl_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/source/include")
set(ClosedGl_SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")

add_executable(ClosedGl)

target_sources(
    ClosedGl PRIVATE
    
    # sources
    "${ClosedGl_SOURCE_DIR}/main.cpp"
    "${ClosedGl_SOURCE_DIR}/mesh.cpp"
    "${ClosedGl_SOURCE_DIR}/tools.cpp"
    "${ClosedGl_SOURCE_DIR}/user_interface.cpp"

    # headers
    "${ClosedGl_INCLUDE_DIR}/closed_gl.hpp"
    "${ClosedGl_INCLUDE_DIR}/mesh.hpp"
    "${ClosedGl_INCLUDE_DIR}/tools.hpp"
    "${ClosedGl_INCLUDE_DIR}/user_interface.hpp"
)

target_include_directories(ClosedGl PRIVATE "${ClosedGl_INCLUDE_DIR}/")

# -----------------------------

# including `FetchContent`
include(FetchContent)

# -----------------------------

# glfw
set(GLFW_LIBRARY_TYPE STATIC)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_EXAMPLES OFF)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG        master
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(glfw)

# -----------------------------

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG        master
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_MakeAvailable(stb)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})


# -----------------------------

# glbinding
set(OPTION_SELF_CONTAINED ON)
set(OPTION_BUILD_TESTS OFF)
set(OPTION_BUILD_DOCS OFF)
set(OPTION_BUILD_TOOLS OFF)
set(OPTION_BUILD_EXAMPLES OFF)
set(OPTION_BUILD_OWN_KHR_HEADERS ON)

FetchContent_Declare(
    glbinding
    GIT_REPOSITORY https://github.com/cginternals/glbinding
    GIT_TAG        master 
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(glbinding)

find_package(glbinding REQUIRED PATHS "${glbinding_SOURCE_DIR}/")

# -----------------------------

# glm
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(glm)

# -----------------------------

# fastgltf
FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(fastgltf)

# -----------------------------

# imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(imgui)

add_library(
    imgui

    # general files
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    
    # platform specific files
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"

    # std::string bindings
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)

target_include_directories(
    imgui PUBLIC
    "${imgui_SOURCE_DIR}" 
    "${imgui_SOURCE_DIR}/backends"
    "${imgui_SOURCE_DIR}/misc/cpp"
    "${glfw_SOURCE_DIR}/include"
)

target_link_libraries(imgui PRIVATE glfw)

# -----------------------------

target_link_libraries(
    ClosedGl PRIVATE
    glbinding::glbinding
    glbinding::glbinding-aux
    glfw
    glm::glm
    fastgltf
    imgui
    stb
)

# -----------------------------

# outputting to the root directory `bin` directory
set_target_properties( 
    ClosedGl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

install(TARGETS ClosedGl)