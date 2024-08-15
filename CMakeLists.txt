cmake_minimum_required(VERSION 3.28)

project(rolling_ball VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  sdl2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-2.30.5
)

FetchContent_MakeAvailable(sdl2)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v5.4.2
)

FetchContent_MakeAvailable(assimp)

if(MSVC)
    add_compile_options(/W3)
else()
  add_compile_options(-Wall -Wextra -pedantic -fno-exceptions -Wunused-result -msse4.1)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-g)
else()
    add_compile_options(-O2)
endif()

if (PROJECT_IS_TOP_LEVEL AND UNIX)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()

add_subdirectory(deps/glad)

include_directories(inc)

include_directories(deps/imgui)
include_directories(deps/imgui/backends)
include_directories(deps/stb)

file(GLOB SOURCES "src/*.cpp")
file(GLOB IMGUI_SOURCES "deps/imgui/*.cpp" "deps/imgui/*.h")

# TODO: shame shame shame shame shame shame shame shame shame shame shame shame shame shame shame shame shame shame
file(GLOB IMGUI_BACKEND_SOURCES "deps/imgui/backends/imgui_impl_sdl2.cpp" "deps/imgui/backends/imgui_impl_sdl2.h" "deps/imgui/backends/imgui_impl_opengl3.h" "deps/imgui/backends/imgui_impl_opengl3.cpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm SDL2::SDL2 assimp::assimp glad)
