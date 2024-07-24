cmake_minimum_required(VERSION 3.22)

project(
    TP4Project
    VERSION 1.0.0
    DESCRIPTION "Aplicación de paralelización con OpenMP para la materia SO2-Practico ii"
    LANGUAGES C
)
set(CMAKE_C_STANDARD 11)

# Directorio para los binarios
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

# Flags de compilación
set(CMAKE_C_FLAGS "-Wall -pedantic -Werror -Wextra -Wconversion -O3 -std=gnu11 -fopenmp")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ") #Para poder usar gdb

# Agregar la librería
add_library(imageUtils STATIC src/imageUtils.c)

# Encuentra la biblioteca GDAL y los archivos de inclusión
find_package(GDAL REQUIRED)

# Incluye las cabeceras de GDAL
include_directories(${GDAL_INCLUDE_DIRS})

# Establecer los flags de compilación para la librería
target_compile_options(imageUtils PRIVATE)

# Establecer los directorios de inclusión para la librería
target_include_directories(imageUtils PUBLIC ${INCLUDE_DIR})

# Crear los ejecutables
add_executable(tp_4 src/tp_4.c)
set_target_properties(tp_4 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Enlazar la librería con los ejecutables del proyecto
target_link_libraries(tp_4 imageUtils ${GDAL_LIBRARIES})

# Target clear
add_custom_target(clear
    COMMAND rm -f ${CMAKE_BINARY_DIR}/tp_4
    COMMAND rm -f ${CMAKE_BINARY_DIR}/border.tif
            ...
    COMMENT "clear"
)
