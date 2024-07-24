cmake_minimum_required(VERSION 3.22)

project(
    TP3Project
    VERSION 1.0.0
    DESCRIPTION "Refactorización de codigo para la materia SO2-Practico ii"
    LANGUAGES C
)
set(CMAKE_C_STANDARD 11)

# Directorio para los binarios
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

# Flags de compilación
set(CMAKE_C_FLAGS "-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11 -O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ") #Para poder usar gdb

# Crear los ejecutables
add_executable(laboratorio3 src/laboratorio3.c)
set_target_properties(laboratorio3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Target clear
add_custom_target(clear
    COMMAND rm -f ${CMAKE_BINARY_DIR}/laboratorio3
            ...
    COMMENT "clear"
)
