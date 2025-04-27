cmake_minimum_required(VERSION 3.9)
project(SDLTester)

set(CMAKE_CXX_STANDARD 11)

# Static linking options (optional but usually good for Windows portability)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# Define your sources clearly
set(SOURCES
        BearGame.cpp         # make sure you actually have a main.cpp!
        SDL_Plotter.cpp
        Soldiers.h
)

# Add the executable
add_executable(SDLTester ${SOURCES})

# Include headers if needed (optional if in same dir)
include_directories(${CMAKE_SOURCE_DIR})

# Link against SDL2
target_link_libraries(SDLTester
        mingw32
        SDL2main
        SDL2
        SDL2_mixer
)
