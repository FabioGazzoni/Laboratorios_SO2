cmake_minimum_required(VERSION 3.22)

project(
    TP5Project
    VERSION 1.0.0
    DESCRIPTION "Servidor http con APIRest para la materia SO2-Practico v"
    LANGUAGES C
)
set(CMAKE_C_STANDARD 11)

# Directorio para los binarios
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

# Flags de compilación
set(CMAKE_C_FLAGS "-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ") #Para poder usar gdb

# Crear los ejecutables
add_executable(server src/server.c)
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Enlazar las librerias necesarias
target_link_libraries(server ulfius orcania yder jansson curl)

# Target clear
add_custom_target(clear
    COMMAND rm -f ${CMAKE_BINARY_DIR}/*
            ...
    COMMENT "clear"
)
