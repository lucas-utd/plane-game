cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(PlaneGame
    VERSION 1.0
    DESCRIPTION "A simple 2D plane shooting game using SFML"
    LANGUAGES CXX
)

# Set the C++ standard to C++20
# and enable C++20 features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find and include the SFML library
find_package(SFML 2.6 COMPONENTS graphics window system network audio REQUIRED)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIRS})
    link_directories(${SFML_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "SFML not found")
endif()

# Find and include the magic_enum library
find_package(magic_enum 0.8.0 REQUIRED)
if (magic_enum_FOUND)
    include_directories(${magic_enum_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "magic_enum not found")
endif()

# Add the source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)
# file(GLOB_RECURSE IMAGES
#     images/*.png
# )
# file(GLOB_RECURSE FONTS
#     fonts/*.ttf
# )
# file(GLOB_RECURSE SOUNDS
#     sounds/*.ogg
# )
# file(GLOB_RECURSE MUSIC
#     music/*.ogg
# )
# Add the include directories
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# set(SOURCES
#     src/*.cpp
# )

# Add the executable target
add_executable(PlaneGame ${SOURCES})

# Add the include directories for the target
target_include_directories(PlaneGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set the compiler flags
if (MSVC)
    target_compile_options(PlaneGame PRIVATE
        /W4
        /permissive-
    )
else()
    target_compile_options(PlaneGame PRIVATE
        -Wall
        -Wextra
        -pedantic
    )
endif()

# Set the C++ standard for the target
set_target_properties(PlaneGame PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Line the SFML and magic enum libraries
target_link_libraries(PlaneGame sfml-system sfml-network sfml-graphics sfml-window sfml-audio magic_enum::magic_enum)

# Set the output directory for the executable
set_target_properties(PlaneGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
)

# Copy the Media directory to the output directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Media DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
