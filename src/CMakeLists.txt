cmake_minimum_required(VERSION 3.7)

project(nesEmu)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(nesEmu main.cpp controls.cpp logging.cpp memory.cpp opcodeFunction.cpp parser.cpp processorExecution.cpp rendering.cpp controls.h globals.h logging.h memory.h opcodeFunction.h parser.h PPUArchitechture.h processorArchitechture.h processorExecution.h rendering.h)
target_link_libraries(nesEmu ${SDL2_LIBRARIES})
