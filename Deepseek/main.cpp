#include <iostream>
#include <cstdio>  // Para FILE, fopen, fclose
#include "KMeansMPI.h"
#include "DataReader.h"

int main(int argc, char** argv) {
    const std::string input_file = "salida";
    
    // Verificar si el archivo de entrada existe
    FILE* file = fopen(input_file.c_str(), "rb");
    if (file) {
        fclose(file);
    } else {
        // Solo el proceso 0 genera los datos de prueba
        int world_rank = 0;
        
        // Inicialización MPI solo para generar datos si es necesario
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        
        if (world_rank == 0) {
            std::cout << "Generando datos de prueba..." << std::endl;
            DataReader::generate_test_data(input_file);
        }
        
        MPI_Finalize();
    }

    // Ejecutar el algoritmo K-means
    KMeansMPI::run(argc, argv, input_file);

    return 0;
}