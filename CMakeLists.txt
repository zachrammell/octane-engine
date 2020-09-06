# CMakeList.txt : CMake project for project-octane, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OCTANE_LIB "octane-lib")
set(OCTANE_GAME "octane-game")
set(OCTANE_TEST "octane-test")

project(${OCTANE_LIB})
project(${OCTANE_GAME})
project(${OCTANE_TEST})

# Handle building libraries
set(BUILD_STATIC_LIBS ON)

add_subdirectory("libraries/SDL")
target_include_directories(SDL2 INTERFACE ${SDL2_INCLUDE_DIR})

add_subdirectory("libraries/EASTL")
target_include_directories(EASTL INTERFACE "libraries/EASTL/include")


file(GLOB_RECURSE OCTANE_LIB_SOURCE CONFIGURE_DEPENDS
  "source/OctaneEngine/**.cpp"
  "source/OctaneEngine/**.h"
)

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
  SDL2-static
  EASTL
)

target_compile_options(${OCTANE_LIB} PRIVATE 
  "/Gr-"
)


file(GLOB_RECURSE OCTANE_GAME_SOURCE CONFIGURE_DEPENDS
  "source/OctaneGame/**.cpp"
  "source/OctaneGame/**.h"
)

add_executable(${OCTANE_GAME} ${OCTANE_GAME_SOURCE})

target_include_directories(${OCTANE_GAME} PRIVATE
  "source"
)

target_link_libraries(${OCTANE_GAME} PRIVATE
  ${OCTANE_LIB}
  SDL2-static
  EASTL
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