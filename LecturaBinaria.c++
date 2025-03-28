#include <iostream>
#include <vector>
#include <fstream>

struct Point {
    float x, y;
};

std::vector<Point> readBinaryData(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo" << std::endl;
        return {};
    }

    uint32_t numRows, numCols;
    file.read(reinterpret_cast<char*>(&numRows), sizeof(numRows));
    file.read(reinterpret_cast<char*>(&numCols), sizeof(numCols));

    if (numCols != 2) {
        std::cerr << "Formato incorrecto, se esperaban 2 columnas." << std::endl;
        return {};
    }

    std::vector<Point> data(numRows);
    file.read(reinterpret_cast<char*>(data.data()), numRows * sizeof(Point));
    file.close();

    return data;
}

int main() {
    std::string filename = "salida";
    std::vector<Point> data = readBinaryData(filename);
    
    if (!data.empty()) {
        std::cout << "Datos leídos correctamente: " << data.size() << " puntos." << std::endl;
        
        for (const auto& point : data) {
            std::cout << point.x << " " << point.y << std::endl;
        }
    }

    return 0;
}