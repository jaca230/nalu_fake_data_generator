cmake_minimum_required(VERSION 3.14)
project(nalu_fake_data_generator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add submodule (must define target `nalu_event_collector`)
add_subdirectory(external/nalu_event_collector)

# Gather source files
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Create the static library
add_library(nalu_fake_data_generator STATIC ${SRC_FILES})

# Add include path for this library (not recursive)
target_include_directories(nalu_fake_data_generator PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link submodule as a dependency
target_link_libraries(nalu_fake_data_generator PUBLIC
  nalu_event_collector
)

# Add executable
add_executable(nalu_fdg_main src/main.cpp)

# Link to your library (which already links `nalu_event_collector`)
target_link_libraries(nalu_fdg_main PRIVATE nalu_fake_data_generator)

# Add headers for executable only (no need to re-add submodule includes)
target_include_directories(nalu_fdg_main PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set output location
set_target_properties(nalu_fdg_main PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Install rules
install(TARGETS nalu_fake_data_generator
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include/nalu_fake_data_generator)
