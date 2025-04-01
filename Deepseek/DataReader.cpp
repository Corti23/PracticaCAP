#include "DataReader.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

std::vector<Point> DataReader::read_binary_data(const std::string& filename, int& n_rows, int& n_cols) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        throw std::runtime_error("No se pudo abrir el archivo");
    }

    file.read(reinterpret_cast<char*>(&n_rows), sizeof(int));
    file.read(reinterpret_cast<char*>(&n_cols), sizeof(int));

    std::vector<float> buffer(n_rows * n_cols);
    file.read(reinterpret_cast<char*>(buffer.data()), n_rows * n_cols * sizeof(float));
    file.close();

    std::vector<Point> data(n_rows);
    #pragma omp parallel for
    for (int i = 0; i < n_rows; ++i) {
        data[i] = Point(n_cols);
        for (int j = 0; j < n_cols; ++j) {
            data[i].coordinates[j] = buffer[i * n_cols + j];
        }
        data[i].cluster_id = -1;
    }

    return data;
}

void DataReader::generate_test_data(const std::string& filename, int n_clusters, int points_per_cluster) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al crear archivo de prueba" << std::endl;
        return;
    }

    std::srand(std::time(nullptr));

    int n_rows = n_clusters * points_per_cluster;
    int n_cols = 2;
    
    file.write(reinterpret_cast<const char*>(&n_rows), sizeof(int));
    file.write(reinterpret_cast<const char*>(&n_cols), sizeof(int));

    for (int i = 0; i < n_clusters; ++i) {
        float x0 = (i % 2) * 10.0f;
        float y0 = (i / 2) * 10.0f;
        
        for (int j = 0; j < points_per_cluster; ++j) {
            float x = x0 + static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
            float y = y0 + static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
            
            file.write(reinterpret_cast<const char*>(&x), sizeof(float));
            file.write(reinterpret_cast<const char*>(&y), sizeof(float));
        }
    }

    file.close();
}