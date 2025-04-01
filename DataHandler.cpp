#include "DataHandler.h"
#include <fstream>
#include <iostream>

std::vector<Point> DataHandler::readBinaryData(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Error al abrir el archivo" << std::endl;
        return {};
    }
    
    uint32_t numRows, numCols;
    file.read(reinterpret_cast<char*>(&numRows), sizeof(numRows));
    file.read(reinterpret_cast<char*>(&numCols), sizeof(numCols));
    
    if (numCols != 2) {
        std::cerr << "Formato incorrecto" << std::endl;
        return {};
    }
    
    std::vector<Point> data(numRows);
    file.read(reinterpret_cast<char*>(data.data()), numRows * sizeof(Point));
    file.close();
    
    return data;
}