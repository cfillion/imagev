cmake_minimum_required(VERSION 3.15)

project(imagev LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL     ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(imagev
  src/comparators.cpp
  src/main.cpp
  src/imagelist.cpp
  src/resources.qrc
)

target_compile_options(imagev PRIVATE
  -Wall -Wextra -Werror
  -fstack-protector-strong -fdiagnostics-color
)

target_link_libraries(imagev Qt5::Core Qt5::Quick)

install(TARGETS imagev)
