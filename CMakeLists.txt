cmake_minimum_required(VERSION 3.14)
project(nalu_fake_data_generator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Do NOT override global output directories here to avoid conflicts in superprojects

# Add submodule (assumes it defines target `nalu_event_collector`)
add_subdirectory(external/nalu_event_collector)

# Gather all source files except main.cpp
file(GLOB_RECURSE ALL_SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Exclude main.cpp explicitly
list(FILTER ALL_SRC_FILES EXCLUDE REGEX ".*/main.cpp$")

# Create static library from source files (excluding main.cpp)
add_library(nalu_fake_data_generator STATIC ${ALL_SRC_FILES})

# Set PIC on for static library in case superproject needs it
set_target_properties(nalu_fake_data_generator PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Public include directories for library consumers
target_include_directories(nalu_fake_data_generator PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/nalu_fake_data_generator>
)

# Link against the submodule library
target_link_libraries(nalu_fake_data_generator PUBLIC
  nalu_event_collector
)

# Create executable from main.cpp
add_executable(nalu_fdg_main ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Link executable to the static library
target_link_libraries(nalu_fdg_main PRIVATE nalu_fake_data_generator)

# Include headers for executable target as well
target_include_directories(nalu_fdg_main PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Optionally set output directory for executable (can be controlled by superproject)
# set_target_properties(nalu_fdg_main PROPERTIES
#   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
# )

# Installation rules

include(GNUInstallDirs)

install(TARGETS nalu_fake_data_generator
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nalu_fake_data_generator
)

install(TARGETS nalu_fdg_main
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nalu_fake_data_generator
)
