cmake_minimum_required(VERSION 3.10)
project(kmeans_serial)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurar includes
include_directories(${CMAKE_SOURCE_DIR}/include)

# Configurar ejecutable
add_executable(kmeans_serial
    src/main.cpp
    src/Point.cpp
    src/Cluster.cpp
    src/DataReader.cpp
    src/Statistics.cpp
    src/KMeans.cpp
)

# Habilitar optimizaciones
target_compile_options(kmeans_serial PRIVATE -O3)