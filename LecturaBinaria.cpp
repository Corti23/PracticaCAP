#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>

int main(int argc, char* argv[]) {
    
    const char* filename = "datos.bin";

    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
        return 1;
    }

    uint32_t num_rows, num_cols;

    // Leer número de filas y columnas
    file.read(reinterpret_cast<char*>(&num_rows), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));

    std::cout << "Filas: " << num_rows << ", Columnas: " << num_cols << std::endl;

    // Reservar memoria para los datos (fila por fila)
    std::vector<std::vector<float>> data(num_rows, std::vector<float>(num_cols));

    // Leer datos
    for (uint32_t i = 0; i < num_rows; ++i) {
        for (uint32_t j = 0; j < num_cols; ++j) {
            file.read(reinterpret_cast<char*>(&data[i][j]), sizeof(float));
        }
    }

    // Mostrar algunos datos para comprobar lectura
    std::cout << "\nPrimeras filas de datos:\n";
    for (uint32_t i = 0; i < std::min(num_rows, 5u); ++i) {
        for (uint32_t j = 0; j < num_cols; ++j) {
            std::cout << std::fixed << std::setprecision(3) << data[i][j] << " ";
        }

        std::cout << "\n";
    }

    return 0;
}