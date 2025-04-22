#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <random>
#include <chrono>
#include <omp.h>  // OpenMP

struct Point {
    std::vector<float> values;
};

bool operator==(const Point& a, const Point& b) {
    if (a.values.size() != b.values.size()) {
        return false;
    }

    for (size_t i = 0; i < a.values.size(); ++i) {
        if (a.values[i] != b.values[i]) {
            return false;
        }
    }
    
    return true;
}

std::vector<Point> readBinaryData(const std::string& filename, uint32_t& numCols) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Error al abrir el archivo binario." << std::endl;
        return {};
    }

    uint32_t numRows;
    file.read(reinterpret_cast<char*>(&numRows), sizeof(numRows));
    file.read(reinterpret_cast<char*>(&numCols), sizeof(numCols));
    std::vector<Point> data(numRows);

    for (uint32_t i = 0; i < numRows; ++i) {
        data[i].values.resize(numCols);
        file.read(reinterpret_cast<char*>(data[i].values.data()), sizeof(float) * numCols);
    }

    return data;
}

float distance(const Point& a, const Point& b) {
    float sum = 0.0f;

    for (size_t i = 0; i < a.values.size(); ++i) {
        sum += (a.values[i] - b.values[i]) * (a.values[i] - b.values[i]);
    }

    return std::sqrt(sum);
}

std::vector<Point> initializeCentroids(const std::vector<Point>& data, int k) {
    std::vector<Point> centroids;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, data.size() - 1);

    for (int i = 0; i < k; ++i) {
        centroids.push_back(data[dist(gen)]);
    }

    return centroids;
}

std::vector<int> assignClusters(const std::vector<Point>& data, const std::vector<Point>& centroids) {
    std::vector<int> labels(data.size());

    #pragma omp parallel for
    for (int i = 0; i < data.size(); ++i) {
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

std::vector<Point> updateCentroids(const std::vector<Point>& data, const std::vector<int>& labels, int k, uint32_t numCols) {
    std::vector<Point> newCentroids(k, Point{std::vector<float>(numCols, 0.0f)});
    std::vector<int> counts(k, 0);

    #pragma omp parallel
    {
        std::vector<Point> localCentroids(k, Point{std::vector<float>(numCols, 0.0f)});
        std::vector<int> localCounts(k, 0);

        #pragma omp for nowait
        for (int i = 0; i < data.size(); ++i) {
            for (uint32_t j = 0; j < numCols; ++j) {
                localCentroids[labels[i]].values[j] += data[i].values[j];
            }

            localCounts[labels[i]]++;
        }

        #pragma omp critical
        for (int c = 0; c < k; ++c) {
            for (uint32_t j = 0; j < numCols; ++j) {
                newCentroids[c].values[j] += localCentroids[c].values[j];
            }

            counts[c] += localCounts[c];
        }
    }

    for (int i = 0; i < k; ++i) {
        if (counts[i] > 0) {
            for (uint32_t j = 0; j < numCols; ++j) {
                newCentroids[i].values[j] /= counts[i];
            }
        }
    }

    return newCentroids;
}

int countDisplacements(const std::vector<int>& oldLabels, const std::vector<int>& newLabels) {
    int count = 0;

    #pragma omp parallel for reduction(+:count)
    for (int i = 0; i < oldLabels.size(); ++i) {
        if (oldLabels[i] != newLabels[i]) {
            count++;
        }
    }
        
    return count;
}

void computeStats(const std::vector<Point>& data, uint32_t numCols) {
    std::vector<float> minVals(numCols, std::numeric_limits<float>::max());
    std::vector<float> maxVals(numCols, std::numeric_limits<float>::lowest());
    std::vector<float> sumVals(numCols, 0.0f);
    std::vector<float> sumSquares(numCols, 0.0f);

    size_t n = data.size();

    #pragma omp parallel for
    for (int i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < numCols; ++j) {
            #pragma omp critical
            {
                float val = data[i].values[j];

                minVals[j] = std::min(minVals[j], val);
                maxVals[j] = std::max(maxVals[j], val);
                sumVals[j] += val;
                sumSquares[j] += val * val;
            }
        }
    }

    for (size_t i = 0; i < numCols; ++i) {
        float mean = sumVals[i] / n;
        float var = (sumSquares[i] / n) - (mean * mean);

        std::cout << "Columna " << i << ": Min=" << minVals[i]
                  << ", Max=" << maxVals[i]
                  << ", Media=" << mean
                  << ", Varianza=" << var << std::endl;
    }
}

int main() {
    uint32_t numCols;
    std::string filename = "salida.bin";
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Point> data = readBinaryData(filename, numCols);

    if (data.empty()) {
        return 1;
    }

    int k = 3;
    std::vector<int> labels(data.size(), -1);
    std::vector<int> newLabels;
    std::vector<Point> centroids = initializeCentroids(data, k);
    std::vector<Point> newCentroids;

    const int maxIterations = 200;

    for (int iter = 0; iter < maxIterations; ++iter) {
        newLabels = assignClusters(data, centroids);

        int moved = countDisplacements(labels, newLabels);

        if (moved < 0.05 * data.size()) {
            break;
        }

        labels = newLabels;
        centroids = updateCentroids(data, labels, k, numCols);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Centroides finales:\n";

    for (const auto& c : centroids) {
        for (float v : c.values) {
            std::cout << v << " ";
        }
            
        std::cout << "\n";
    }

    std::cout << "Tiempo de ejecucion: " << duration.count() << " ms\n";

    std::cout << "\nEstadisticas por columna:\n";
    computeStats(data, numCols);

    return 0;
}