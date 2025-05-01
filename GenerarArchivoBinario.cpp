#include <iostream>
#include <fstream>
#include <random>
#include <cstdint>

int main() {
    uint32_t num_rows, num_cols;
    std::string filename;

     // Entrada por pantalla
     std::cout << "Introduce el número de filas: ";
     std::cin >> num_rows;
 
     std::cout << "Introduce el número de columnas: ";
     std::cin >> num_cols;
 
     std::cout << "Introduce el nombre del archivo de salida (por ejemplo: datos.bin): ";
     std::cin >> filename;

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
    std::cout << "Archivo '" << filename << "' generado con exito con " << num_rows << " filas y " << num_cols << " columnas." << std::endl;

    return 0;
}