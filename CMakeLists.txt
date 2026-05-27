cmake_minimum_required(VERSION 3.10)
project(chip8_emu C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

add_executable(chip8 src/chip8.c src/main.c)

target_include_directories(chip8 PRIVATE src)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)

add_custom_target(link_roms ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${CMAKE_CURRENT_SOURCE_DIR}/roms"
    "${CMAKE_BINARY_DIR}/roms"
    COMMENT "Ensuring ROMs directory is linked to build folder"
    VERBATIM
)