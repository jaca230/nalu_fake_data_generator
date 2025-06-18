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

# Gather all source files except main.cpp
file(GLOB_RECURSE ALL_SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Exclude main.cpp explicitly
list(FILTER ALL_SRC_FILES EXCLUDE REGEX ".*/main.cpp$")

# Create the static library without main.cpp
add_library(nalu_fake_data_generator STATIC ${ALL_SRC_FILES})

# Include directory for the library
target_include_directories(nalu_fake_data_generator PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link submodule as dependency
target_link_libraries(nalu_fake_data_generator PUBLIC
  nalu_event_collector
)

# Now create the executable from main.cpp only
add_executable(nalu_fdg_main main.cpp)

# Link executable to the static library
target_link_libraries(nalu_fdg_main PRIVATE nalu_fake_data_generator)

# Include directories for executable (headers in include/)
target_include_directories(nalu_fdg_main PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set output location for executable
set_target_properties(nalu_fdg_main PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Install rules for the library and headers
install(TARGETS nalu_fake_data_generator
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include/nalu_fake_data_generator)
