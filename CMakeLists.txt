cmake_minimum_required(VERSION 3.24)
project(CodeQuest VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(raylib 6.0 CONFIG REQUIRED)
add_executable(codequest src/main.c src/screens.c src/ui.c)
target_link_libraries(codequest PRIVATE raylib)
add_custom_command(TARGET codequest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:codequest>/assets")
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(codequest PRIVATE -Wall -Wextra -Wpedantic)
endif()
