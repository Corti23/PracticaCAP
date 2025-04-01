#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <random>
#include <fstream>
// Biblioteca para medir el tiempo
#include <chrono>  

struct Point {
    float x, y;
};

// Operador de comparación para evitar error en la condición de convergencia
bool operator==(const Point& a, const Point& b) {
    return (a.x == b.x) && (a.y == b.y);
}

// Función de distancia Euclidiana
float distance(const Point& a, const Point& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Función para leer datos binarios
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
    std::vector<float> rawData(numRows * numCols);
    
    file.read(reinterpret_cast<char*>(rawData.data()), numRows * numCols * sizeof(float));
    file.close();

    for (size_t i = 0; i < numRows; i++) {
        data[i].x = rawData[i * numCols];
        data[i].y = rawData[i * numCols + 1];
    }

    return data;
}

// Función para inicializar centroides aleatorios
std::vector<Point> initializeCentroids(const std::vector<Point>& data, int k) {
    if (data.empty()) {
        std::cerr << "Error: el conjunto de datos está vacío." << std::endl;
        exit(1);
    }

    std::vector<Point> centroids;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, data.size() - 1);
    
    for (int i = 0; i < k; ++i) {
        centroids.push_back(data[dist(gen)]);
    }

    return centroids;
}

// Asignar cada punto al cluster más cercano
std::vector<int> assignClusters(const std::vector<Point>& data, const std::vector<Point>& centroids) {
    std::vector<int> labels(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        float minDist = std::numeric_limits<float>::max();
        int bestCluster = 0;

        for (size_t j = 0; j < centroids.size(); ++j) {
            float d = distance(data[i], centroids[j]);

            if (d < minDist) {
                minDist = d;
                bestCluster = j;
            }
        }

        labels[i] = bestCluster;
    }

    return labels;
}

// Recalcular los centroides
std::vector<Point> updateCentroids(const std::vector<Point>& data, const std::vector<int>& labels, int k) {
    std::vector<Point> newCentroids(k, {0, 0});
    std::vector<int> counts(k, 0);
    
    for (size_t i = 0; i < data.size(); ++i) {
        newCentroids[labels[i]].x += data[i].x;
        newCentroids[labels[i]].y += data[i].y;
        counts[labels[i]]++;
    }
    
    for (int i = 0; i < k; ++i) {
        if (counts[i] > 0) {
            newCentroids[i].x /= counts[i];
            newCentroids[i].y /= counts[i];
        }
    }

    return newCentroids;
}

   void kmeans(std::vector<Point>& data, int k) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<Point> centroids = initializeCentroids(data, k);
    std::vector<int> labels;
    std::vector<Point> newCentroids;
    
    for (int i = 0; i < 10; ++i) { 
        labels = assignClusters(data, centroids);
        newCentroids = updateCentroids(data, labels, k);
        if (newCentroids == centroids) break;
        centroids = newCentroids;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "Centroides finales:\n";
    for (const auto& c : centroids) {
        std::cout << c.x << " " << c.y << std::endl;
    }
    std::cout << "Tiempo de ejecución: " << duration.count() << " segundos\n";
}

int main() {
    std::string filename = "salida.bin";
    std::vector<Point> data = readBinaryData(filename);
    if (data.empty()) return 1;
    
    int k = 2;
    kmeans(data, k);
    
    return 0;
}
/*
// Point.h
#ifndef POINT_H
#define POINT_H

struct Point {
    float x, y;
    bool operator==(const Point& other) const;
};

float distance(const Point& a, const Point& b);

#endif

// Point.cpp
#include "Point.h"
#include <cmath>

bool Point::operator==(const Point& other) const {
    return (x == other.x) && (y == other.y);
}

float distance(const Point& a, const Point& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// DataHandler.h
#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "Point.h"
#include <vector>
#include <string>

class DataHandler {
public:
    static std::vector<Point> readBinaryData(const std::string& filename);
};

#endif

// DataHandler.cpp
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

// KMeans.h
#ifndef KMEANS_H
#define KMEANS_H

#include "Point.h"
#include <vector>

class KMeans {
public:
    KMeans(int k);
    void fit(std::vector<Point>& data);
private:
    int k;
    std::vector<Point> initializeCentroids(const std::vector<Point>& data);
    std::vector<int> assignClusters(const std::vector<Point>& data, const std::vector<Point>& centroids);
    std::vector<Point> updateCentroids(const std::vector<Point>& data, const std::vector<int>& labels);
};

#endif

// KMeans.cpp
#include "KMeans.h"
#include <random>
#include <limits>
#include <iostream>
#include <chrono>

KMeans::KMeans(int k) : k(k) {}

void KMeans::fit(std::vector<Point>& data) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Point> centroids = initializeCentroids(data);
    std::vector<int> labels;
    std::vector<Point> newCentroids;
    
    for (int i = 0; i < 10; ++i) {
        labels = assignClusters(data, centroids);
        newCentroids = updateCentroids(data, labels);
        if (newCentroids == centroids) break;
        centroids = newCentroids;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "Centroides finales:\n";
    for (const auto& c : centroids) {
        std::cout << c.x << " " << c.y << std::endl;
    }
    std::cout << "Tiempo de ejecución: " << duration.count() << " segundos\n";
}

std::vector<Point> KMeans::initializeCentroids(const std::vector<Point>& data) {
    std::vector<Point> centroids;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, data.size() - 1);
    for (int i = 0; i < k; ++i) {
        centroids.push_back(data[dist(gen)]);
    }
    return centroids;
}

std::vector<int> KMeans::assignClusters(const std::vector<Point>& data, const std::vector<Point>& centroids) {
    std::vector<int> labels(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        float minDist = std::numeric_limits<float>::max();
        int bestCluster = 0;
        for (size_t j = 0; j < centroids.size(); ++j) {
            float d = distance(data[i], centroids[j]);
            if (d < minDist) {
                minDist = d;
                bestCluster = j;
            }
        }
        labels[i] = bestCluster;
    }
    return labels;
}

std::vector<Point> KMeans::updateCentroids(const std::vector<Point>& data, const std::vector<int>& labels) {
    std::vector<Point> newCentroids(k, {0, 0});
    std::vector<int> counts(k, 0);
    for (size_t i = 0; i < data.size(); ++i) {
        newCentroids[labels[i]].x += data[i].x;
        newCentroids[labels[i]].y += data[i].y;
        counts[labels[i]]++;
    }
    for (int i = 0; i < k; ++i) {
        if (counts[i] > 0) {
            newCentroids[i].x /= counts[i];
            newCentroids[i].y /= counts[i];
        }
    }
    return newCentroids;
}

// main.cpp
#include "DataHandler.h"
#include "KMeans.h"
#include <iostream>

int main() {
    std::string filename = "salida.bin";
    std::vector<Point> data = DataHandler::readBinaryData(filename);
    if (data.empty()) return 1;
    
    int k = 2;
    KMeans kmeans(k);
    kmeans.fit(data);
    
    return 0;
}
*/