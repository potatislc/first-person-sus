cmake_minimum_required(VERSION 3.22.1)
project(first-person-sus)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

set(EXE_NAME ${PROJECT_NAME})

include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.2.12
)

# Disable SDL extras BEFORE MakeAvailable
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)        # Tests
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)     # Example programs
set(SDL_INSTALL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_DOCS OFF CACHE BOOL "" FORCE)         # Doxygen stuff
set(SDL_CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE) # Avoids "d" postfix

# (optional) Don’t build libSDLmain if you don't need it
set(SDL_MAIN_HANDLED ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(${EXE_NAME} main.cpp)
target_link_libraries(${EXE_NAME} PRIVATE SDL3::SDL3 glad::glad glm::glm)

get_target_property(SDL3_INCLUDE_DIRS SDL3::SDL3 INTERFACE_INCLUDE_DIRECTORIES)
if (SDL3_INCLUDE_DIRS)
    target_include_directories(${EXE_NAME} SYSTEM PRIVATE ${SDL3_INCLUDE_DIRS})
endif()

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
)

target_compile_options(${EXE_NAME} PRIVATE ${WARNING_FLAGS})

# Turn off RTTI and exceptions
if (MSVC)
    target_compile_options(${EXE_NAME} PRIVATE /GR- /EHs-c- /we4530 /we4577)
else()
    target_compile_options(${EXE_NAME} PRIVATE -fno-exceptions -fno-rtti -Werror=non-virtual-dtor -Werror=rtti -Werror=exceptions)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    # Add sanitizers
    target_compile_options(${EXE_NAME} PRIVATE -fsanitize=address,undefined -g)
    target_link_options(${EXE_NAME} PRIVATE -fsanitize=address,undefined)

    if (MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_link_libraries(${EXE_NAME} PRIVATE
                clang_rt.asan_dynamic-x86_64
                clang_rt.asan_dynamic_runtime_thunk-x86_64
        )
        target_link_options(${EXE_NAME} PRIVATE
                /wholearchive:clang_rt.asan_dynamic_runtime_thunk-x86_64.lib
        )
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${EXE_NAME} PRIVATE -O3 -DNDEBUG)
endif()
