#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
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

float distance(const Point& a, const Point& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

std::vector<Point> initializeCentroids(const std::vector<Point>& data, int k) {
    std::vector<Point> centroids(k);
    
    for (int i = 0; i < k; ++i) {
        centroids[i] = data[rand() % data.size()];
    }

    return centroids;
}

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

void kMeans(std::vector<Point>& data, int k, int maxIterations = 100) {
    std::vector<Point> centroids = initializeCentroids(data, k);
    std::vector<int> labels;
    
    for (int iter = 0; iter < maxIterations; ++iter) {
        labels = assignClusters(data, centroids);
        std::vector<Point> newCentroids = updateCentroids(data, labels, k);

        if (centroids == newCentroids) {
            break; // Converged
        }

        centroids = newCentroids;
    }
    
    std::cout << "Centroides finales:" << std::endl;

    for (const auto& c : centroids) {
        std::cout << c.x << " " << c.y << std::endl;
    }
}

int main() {
    std::string filename = "salida";
    std::vector<Point> data = readBinaryData(filename);

    if (data.empty()) {
        return 1;
    }
    
    int k = 2;
    kMeans(data, k);

    return 0;
}