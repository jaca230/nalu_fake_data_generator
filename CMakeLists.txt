cmake_minimum_required(VERSION 3.14)
project(nalu_fake_data_generator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Include all subdirectories under include/
file(GLOB_RECURSE ALL_INCLUDE_PATHS LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/include/*)
foreach(path ${ALL_INCLUDE_PATHS})
    if(IS_DIRECTORY ${path})
        include_directories(${path})
    endif()
endforeach()

# Collect and print all header files
file(GLOB_RECURSE HEADER_FILES "include/**/*.h")
message(STATUS "Header files found:")
foreach(header ${HEADER_FILES})
    message(STATUS "  ${header}")
endforeach()

# Recursively collect source files under src/
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Paths for external dependency nalu_event_collector
set(NAEC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/nalu_event_collector/include")
set(NAEC_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/external/nalu_event_collector/lib/libnalu_event_collector.a")

# Create the static library
add_library(nalu_fake_data_generator STATIC ${SRC_FILES})

# Include directories for the static library
target_include_directories(nalu_fake_data_generator PUBLIC
  ${NAEC_INCLUDE_DIR}
)

# Link against the external static library (must be built beforehand)
target_link_libraries(nalu_fake_data_generator PUBLIC
  ${NAEC_LIBRARY}
)

# Install rules (optional)
install(TARGETS nalu_fake_data_generator
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include/nalu_fake_data_generator)

# Add an executable target
add_executable(nalu_fdg_main src/main.cpp)

# Link the library to the executable
target_link_libraries(nalu_fdg_main PRIVATE nalu_fake_data_generator)

# Include directories for the executable
target_include_directories(nalu_fdg_main PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Set output directory for the executable
set_target_properties(nalu_fdg_main PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
