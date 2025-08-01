cmake_minimum_required(VERSION 3.22.1)
project(first-person-sus)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif ()

set(EXE_NAME ${PROJECT_NAME})

include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.12
)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_DOCS OFF CACHE BOOL "" FORCE)
set(SDL_CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
set(SDL_MAIN_HANDLED ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking  # Use latest docking branch with SDL3 support
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_backend STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

target_include_directories(imgui_backend PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_backend PRIVATE SDL3::SDL3)

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(${EXE_NAME} engine/src/example/main.cpp
        engine/src/renderer/GlRenderer.cpp
        engine/src/renderer/shader/Source.cpp
        engine/src/renderer/shader/Program.cpp
        engine/src/renderer/shader/Parser.cpp
        engine/src/renderer/buffer/Vertex.cpp
        engine/src/renderer/buffer/Index.cpp
        engine/src/renderer/VertexArray.cpp
        engine/src/renderer/shader/Uniform.cpp
        engine/src/vendor/stb_image/stb_image.cpp
        engine/src/renderer/Texture.cpp
        engine/src/scene/test/Test.cpp
        engine/src/core/Application.cpp
        engine/src/scene/node/Node.h
        engine/src/scene/node/Node.cpp
        engine/src/scene/test/Cube.cpp
        engine/src/renderer/Camera.cpp
        engine/src/scene/test/Cube2.cpp
        engine/src/core/InputMap.cpp
        engine/src/renderer/model/Model.cpp
        engine/src/renderer/model/ObjParser.cpp)

target_link_libraries(${EXE_NAME} PRIVATE SDL3::SDL3 glad::glad glm::glm imgui_backend assimp::assimp)

target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/engine/res")


target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/src
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        engine/src/vendor/stb_image
)

file(GLOB STB_IMAGE_SRC "engine/src/vendor/stb_image/stb_image.cpp")

set_source_files_properties(${STB_IMAGE_SRC}
        PROPERTIES COMPILE_FLAGS "-w"
)

set(WARNING_FLAGS
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wno-unused-parameter
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-switch-default
)

target_compile_options(${EXE_NAME} PRIVATE ${WARNING_FLAGS})


if (MSVC)
    target_compile_options(${EXE_NAME} PRIVATE /GR- /EHs-c- /we4530 /we4577)
else ()
    target_compile_options(${EXE_NAME} PRIVATE -fno-exceptions -fno-rtti -Werror=non-virtual-dtor -Werror=rtti -Werror=exceptions)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(${EXE_NAME} PRIVATE -O0 -g -fsanitize=address,undefined)
    target_link_options(${EXE_NAME} PRIVATE -fsanitize=address,undefined /INCREMENTAL:NO)

    if (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_link_libraries(${EXE_NAME} PRIVATE
                clang_rt.asan_dynamic-x86_64
                clang_rt.asan_dynamic_runtime_thunk-x86_64
        )
        target_link_options(${EXE_NAME} PRIVATE
                /wholearchive:clang_rt.asan_dynamic_runtime_thunk-x86_64.lib
        )
    endif ()
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${EXE_NAME} PRIVATE -O3 -DNDEBUG)
endif ()

if (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_target_properties(${EXE_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif ()
