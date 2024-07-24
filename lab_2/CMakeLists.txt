cmake_minimum_required(VERSION 3.22)

project(
    TP2Project
    VERSION 1.0.0
    DESCRIPTION "Aplicacion-servidor para la materia SO2-Practico ii"
    LANGUAGES C
)
set(CMAKE_C_STANDARD 11)

# Directorio para los binarios
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

# Flags de compilación
set(CMAKE_C_FLAGS "-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ") #Para poder usar gdb


# Directorio para los include
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Agregar la librería
add_library(Utils STATIC src/common_utils.c)
add_library(cJSON STATIC src/cJSON.c)

# Para enlazar la libreria OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(Utils PRIVATE OpenSSL::SSL bluetooth)

# Para enlazar la libreria de ZLIB
find_package(ZLIB REQUIRED)

# Establecer los flags de compilación para la librería
target_compile_options(cJSON PRIVATE)
target_compile_options(Utils PRIVATE)

# Establecer los directorios de inclusión para la librería
target_include_directories(cJSON PUBLIC ${INCLUDE_DIR})
target_include_directories(Utils PUBLIC ${INCLUDE_DIR})

# Establecer los header
set_target_properties(cJSON PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/cJSON.h")


# Crear los ejecutables
add_executable(cliente_internet src/cliente_internet.c)
set_target_properties(cliente_internet PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(cliente_unix src/cliente_unix.c)
set_target_properties(cliente_unix PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(cliente_bluetooth src/cliente_bluetooth.c)
set_target_properties(cliente_bluetooth PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(servidor src/servidor.c)
set_target_properties(servidor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Enlazar la librería con los ejecutables del proyecto
target_link_libraries(servidor PRIVATE Utils cJSON ${ZLIB_LIBRARIES})
target_link_libraries(cliente_internet PRIVATE Utils cJSON)
target_link_libraries(cliente_unix PRIVATE Utils cJSON ${ZLIB_LIBRARIES})
target_link_libraries(cliente_bluetooth PRIVATE Utils cJSON)

# Target clear
add_custom_target(clear
    COMMAND rm -f ${CMAKE_BINARY_DIR}/servidor
    COMMAND rm -f ${CMAKE_BINARY_DIR}/cliente_internet
    COMMAND rm -f ${CMAKE_BINARY_DIR}/cliente_unix
    COMMAND rm -f ${CMAKE_BINARY_DIR}/cliente_bluetooth
            ...
    COMMENT "clear"
)
