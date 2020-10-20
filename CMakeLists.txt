# CMakeList.txt : CMake project for project-octane, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OCTANE_LIB "octane-lib")
set(OCTANE_GAME "octane-game")
set(OCTANE_TEST "octane-test")

project("Octane")

# Put everything in the same folder so that we don't need to copy DLLs around
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(BUILD_SHARED_LIBS ON)

# Handle building libraries
add_subdirectory("libraries/SDL")
target_include_directories(SDL2 INTERFACE ${SDL_INCLUDE_DIR})

set(BUILD_SHARED_LIBS OFF)

add_subdirectory("libraries/EASTL")
target_include_directories(EASTL INTERFACE "libraries/EASTL/include")

# Build IMGUI
set(IMGUI_DIR "libraries/imgui")
set(IMGUI_INCLUDE_DIR ${IMGUI_DIR} "${IMGUI_DIR}/examples")
file(GLOB IMGUI_SRC "${IMGUI_DIR}/*.cpp")
# Append IMGUI SRC with dx11 and SDL
set(IMGUI_SRC
  ${IMGUI_SRC}
  ${IMGUI_DIR}/examples/imgui_impl_dx11.cpp
  ${IMGUI_DIR}/examples/imgui_impl_sdl.cpp)
add_library(IMGUI ${IMGUI_SRC})
target_include_directories(IMGUI PUBLIC ${IMGUI_INCLUDE_DIR})
target_link_libraries(IMGUI
  PRIVATE
  d3d11
  dxgi
  d3dcompiler
  SDL2
)

# Wwise
set(WWISE_DIR "libraries/Wwise")
set(WWISE_INCLUDE_DIR "${WWISE_DIR}" "${WWISE_DIR}/SampleSoundEngine/Common")
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
  file(GLOB WWISE_BINARIES "${WWISE_DIR}/Debug/lib/*.lib")
else()
  file(GLOB WWISE_BINARIES "${WWISE_DIR}/Release/lib/*.lib")
endif()
file(GLOB WWISE_SAMPLE_SOURCES "${WWISE_DIR}/SampleSoundEngine/*.cpp")
if (WIN32)
  file(GLOB WWISE_SAMPLE_SOURCES "${WWISE_DIR}/SampleSoundEngine/Win32/*.cpp")
  set(WWISE_INCLUDE_DIR "${WWISE_INCLUDE_DIR}" "${WWISE_DIR}/SampleSoundEngine/Win32")
endif()
add_library(WWISE STATIC ${WWISE_BINARIES} ${WWISE_SAMPLE_SOURCES})
target_compile_definitions(WWISE PRIVATE UNICODE)
set_target_properties(WWISE PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(WWISE PUBLIC ${WWISE_INCLUDE_DIR})

file(GLOB_RECURSE OCTANE_LIB_SOURCE CONFIGURE_DEPENDS
  "source/OctaneEngine/**.cpp"
  "source/OctaneEngine/**.h"
)

# assimp
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
add_subdirectory("libraries/assimp")

# magic enum
add_subdirectory("libraries/magic_enum")

# Add source to the library
add_library(${OCTANE_LIB} ${OCTANE_LIB_SOURCE})

target_include_directories(${OCTANE_LIB} PRIVATE
  "source"
)

target_link_libraries(${OCTANE_LIB} PRIVATE
# system libraries
  user32
  d3d11
  dxgi
  d3dcompiler
# external libraries
  SDL2
  EASTL
  IMGUI
  WWISE
  assimp
  magic_enum
)

file(GLOB_RECURSE OCTANE_GAME_SOURCE CONFIGURE_DEPENDS
  "source/OctaneGame/**.cpp"
  "source/OctaneGame/**.h"
)

add_executable(${OCTANE_GAME} ${OCTANE_GAME_SOURCE})

#remove /W3 and /GR flags since we're using /GR- and /W4
#string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})


target_compile_options(${OCTANE_LIB} PRIVATE
#no exceptions
  "/GR-"
#warning level
  "/W3"
#warnings as errors
  #"/WX"
)

target_compile_options(${OCTANE_GAME} PRIVATE
#no exceptions
  "/GR-"
#warning level
  "/W3"
#warnings as errors
  #"/WX"
)

target_include_directories(${OCTANE_GAME} PRIVATE
  "source"
)

target_link_libraries(${OCTANE_GAME} PRIVATE
  ${OCTANE_LIB}
  SDL2
  EASTL
  IMGUI
)

file(GLOB_RECURSE OCTANE_TEST_SOURCE CONFIGURE_DEPENDS
  "source/OctaneTest/**.cpp"
  "source/OctaneTest/**.h"
)

add_executable(${OCTANE_TEST} ${OCTANE_TEST_SOURCE})

target_include_directories(${OCTANE_TEST} PRIVATE
  "source"
  "libraries/catch2/include"
)

target_link_libraries(${OCTANE_TEST} PRIVATE
  ${OCTANE_LIB}
)