cmake_minimum_required(VERSION 3.10)
project("N body Simulation" C)

# Specify the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define executable name
set(EXECUTABLE_NAME Nobody)

# Set platform-specific options
if(WIN32)
    # Windows specific settings
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33")
    set(RAYLIB_LIBS_PATH "${PROJECT_SOURCE_DIR}/external/raylib/src")
    set(RAYLIB_LIB "${RAYLIB_LIBS_PATH}/libraylib.a")
    set(SYSTEM_LIBS opengl32 gdi32 winmm)
elseif(APPLE)
    # macOS specific settings
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33")
    set(RAYLIB_LIBS_PATH "${PROJECT_SOURCE_DIR}/external/raylib/src")
    set(RAYLIB_LIB "${RAYLIB_LIBS_PATH}/libraylib.a")
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    set(SYSTEM_LIBS ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY} "-framework OpenGL" "-framework CoreFoundation" ${SDL2_LIBRARIES})
elseif(UNIX)
    # Linux specific settings
    set(CMAKE_C_FLAGS "-O2 ${CMAKE_C_FLAGS} -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33")
    set(RAYLIB_LIBS_PATH "${PROJECT_SOURCE_DIR}/external/raylib/src")
    set(RAYLIB_LIB "${RAYLIB_LIBS_PATH}/libraylib.a")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    set(SYSTEM_LIBS GL m pthread ${SDL2_LIBRARIES})

    # Add SDL2 include directories
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

# Add Raylib include directory
include_directories(${PROJECT_SOURCE_DIR}/external/raylib/src)

# Add source files
add_executable(${EXECUTABLE_NAME}
    src/main.c
    src/utils.c
    src/vector.c
    src/body.c
    src/tree.c
)

# Link libraries
target_link_libraries(${EXECUTABLE_NAME} ${RAYLIB_LIB} ${SYSTEM_LIBS})