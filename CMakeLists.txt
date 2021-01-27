cmake_minimum_required(VERSION 3.15)
project(imagev LANGUAGES CXX)

include(CheckIncludeFileCXX)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/options.cpp
  src/player.cpp
  src/playlist.cpp
  src/window.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED ON
  CXX_VISIBILITY_PRESET "hidden"
  VISIBILITY_INLINES_HIDDEN ON

  INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON
  INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON
  INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL     ON
)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall -Wextra -Werror
  -fstack-protector-strong -fdiagnostics-color
)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(MPV REQUIRED IMPORTED_TARGET mpv)
target_link_libraries(${PROJECT_NAME} PkgConfig::GLFW PkgConfig::MPV)

check_include_file_cxx(filesystem HAS_STD_FILESYSTEM)
if(HAS_STD_FILESYSTEM)
  target_compile_definitions(${PROJECT_NAME} PUBLIC HAS_STD_FILESYSTEM)
else()
  find_package(Boost COMPONENTS filesystem REQUIRED)
  target_link_libraries(${PROJECT_NAME} Boost::filesystem)
endif()

if(APPLE)
  target_sources(${PROJECT_NAME} PRIVATE src/filesystem.mm src/window.mm)

  find_library(APPKIT_LIB AppKit)
  target_link_libraries(${PROJECT_NAME} ${APPKIT_LIB})
else()
  target_sources(${PROJECT_NAME} PRIVATE src/filesystem.cpp)
endif()

find_package(ICU COMPONENTS i18n uc)
if(ICU_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/comparators_icu.cpp)
  target_link_libraries(${PROJECT_NAME} ICU::i18n ICU::uc)
elseif(APPLE)
  target_sources(${PROJECT_NAME} PRIVATE src/comparators.mm)
else()
  message(FATAL_ERROR "Missing collator implementation. Is ICU installed?")
endif()

install(TARGETS ${PROJECT_NAME})
