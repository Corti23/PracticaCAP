#include <iostream>
#include <fstream>
#include <random>
#include <cstdint>

int main() {
    const uint32_t num_rows = 100;
    const uint32_t num_cols = 3;
    const char* filename = "datos.bin";

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "No se pudo crear el archivo." << std::endl;
        return 1;
    }

    // Escribir número de filas y columnas
    file.write(reinterpret_cast<const char*>(&num_rows), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&num_cols), sizeof(uint32_t));

    // Generador de números aleatorios (valores entre 0 y 100)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 100.0f);

    // Escribir los valores fila por fila
    for (uint32_t i = 0; i < num_rows; ++i) {
        for (uint32_t j = 0; j < num_cols; ++j) {
            float value = dist(gen);
            file.write(reinterpret_cast<const char*>(&value), sizeof(float));
        }
    }

    file.close();
    std::cout << "Archivo '" << filename << "' generado con exito." << std::endl;

    return 0;
}