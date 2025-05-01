cmake_minimum_required(VERSION 3.23)
project(engine3D C)

set(CMAKE_C_STANDARD 99)

add_executable(engine3D main.c )
# Aggiungi il percorso della cartella degli header SDL2
include_directories("${CMAKE_SOURCE_DIR}/folder/include")

# Aggiungi il percorso della cartella delle librerie SDL2
link_directories("${CMAKE_SOURCE_DIR}/folder/lib")

# Collega le librerie SDL2
target_link_libraries(engine3D SDL2 SDL2main)
