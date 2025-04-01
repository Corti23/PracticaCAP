#include "DataGenerator.h"
#include <iostream>
#include <fstream>
#include <random>

#define PI 3.14159265358979323846f

Point getRandomPoint(float x0, float y0, float maxRadius, float minRadius = 0.0f) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distRadius(minRadius, maxRadius);
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * PI);

    float r = distRadius(gen);
    float alpha = distAngle(gen);

    return {x0 + r * cos(alpha), y0 + r * sin(alpha)};
}

void DataGenerator::generateData(const std::string& filename, int nClusters, int nPointsPerCluster) {
    std::vector<Point> data;

    for (int i = 0; i < nClusters; i++) {
        Point centroid = getRandomPoint(0.0f, 0.0f, 20.0f);

        for (int j = 0; j < nPointsPerCluster; j++) {
            data.push_back(getRandomPoint(centroid.x, centroid.y, 1.0f));
        }
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo para escritura." << std::endl;
        return;
    }

    int nFilas = nClusters * nPointsPerCluster;
    int nCol = 2;
    file.write(reinterpret_cast<char*>(&nFilas), sizeof(int));
    file.write(reinterpret_cast<char*>(&nCol), sizeof(int));
    file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(Point));
    file.close();
}

std::vector<Point> DataGenerator::readBinaryData(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo." << std::endl;
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
