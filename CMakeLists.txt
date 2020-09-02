# CMakeList.txt : CMake project for project-octane, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.12)

set(PROJECT_OCTANE "project-octane")

project(${PROJECT_OCTANE})

# Handle building libraries
set(BUILD_STATIC_LIBS ON)

add_subdirectory("libraries/SDL")
target_include_directories(SDL2 INTERFACE ${SDL2_INCLUDE_DIR})

add_subdirectory("libraries/EASTL")
target_include_directories(EASTL INTERFACE "libraries/EASTL/include")


file(GLOB_RECURSE OCTANE_SOURCE CONFIGURE_DEPENDS
  "source/project-octane/**.cpp"
  "source/project-octane/**.h"
)

# Add source to this project's executable.
add_executable(${PROJECT_OCTANE} ${OCTANE_SOURCE})

target_include_directories(
  ${PROJECT_OCTANE}
  PRIVATE "source"
  PUBLIC ${SDL2_INCLUDE_DIR}
)

target_link_libraries(
  ${PROJECT_OCTANE}
  SDL2-static
  EASTL
)

target_compile_options(
  ${PROJECT_OCTANE}
  PRIVATE "/Gr-"
)
