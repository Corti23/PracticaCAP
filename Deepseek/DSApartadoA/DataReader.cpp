#include "DataReader.h"
#include <fstream>
#include <iostream>

std::vector<Point> DataReader::read_binary_data(const std::string& filename, int& n_rows, int& n_cols) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        exit(1);
    }

    file.read(reinterpret_cast<char*>(&n_rows), sizeof(int));
    file.read(reinterpret_cast<char*>(&n_cols), sizeof(int));

    std::vector<float> buffer(n_rows * n_cols);
    file.read(reinterpret_cast<char*>(buffer.data()), n_rows * n_cols * sizeof(float));
    file.close();

    std::vector<Point> data(n_rows, Point(n_cols));
    for (int i = 0; i < n_rows; ++i) {
        for (int j = 0; j < n_cols; ++j) {
            data[i].coordinates[j] = buffer[i * n_cols + j];
        }
    }

    return data;
}