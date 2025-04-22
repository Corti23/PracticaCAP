#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>
#include <random>
#include <mpi.h>

struct Point {
    std::vector<float> values;
};

float distance(const Point& a, const Point& b) {
    float sum = 0.0f;
    
    for (size_t i = 0; i < a.values.size(); ++i) {
        sum += (a.values[i] - b.values[i]) * (a.values[i] - b.values[i]);
    }
        
    return std::sqrt(sum);
}

std::vector<Point> readBinaryDataPartition(const std::string& filename, uint32_t& numCols, int rank, int size) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        if (rank == 0) {
            std::cerr << "Error al abrir el archivo binario." << std::endl;
        }

        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    uint32_t numRows;
    file.read(reinterpret_cast<char*>(&numRows), sizeof(numRows));
    file.read(reinterpret_cast<char*>(&numCols), sizeof(numCols));

    int rowsPerProcess = numRows / size;
    int remainder = numRows % size;
    int localRows = rowsPerProcess + (rank < remainder ? 1 : 0);
    int offset = rank * rowsPerProcess + std::min(rank, remainder);

    file.seekg(sizeof(float) * numCols * offset, std::ios::cur);

    std::vector<Point> data(localRows);

    for (int i = 0; i < localRows; ++i) {
        data[i].values.resize(numCols);
        file.read(reinterpret_cast<char*>(data[i].values.data()), sizeof(float) * numCols);
    }

    file.close();

    return data;
}

Point computeCentroid(const std::vector<Point>& data, uint32_t numCols) {
    Point centroid;
    centroid.values.resize(numCols, 0.0f);

    for (const auto& p : data) {
        for (size_t i = 0; i < numCols; ++i) {
            centroid.values[i] += p.values[i];
        }
    }

    for (size_t i = 0; i < numCols; ++i) {
        centroid.values[i] /= data.size();
    }
        
    return centroid;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) {
            std::cerr << "Uso: " << argv[0] << " <archivo_binario>" << std::endl;
        }

        MPI_Finalize();

        return 1;
    }

    std::string filename = argv[1];
    uint32_t numCols;
    std::vector<Point> localData = readBinaryDataPartition(filename, numCols, rank, size);

    // Paso 1: calcular centroide local
    Point localCentroid = computeCentroid(localData, numCols);

    // Paso 2: compartir todos los centroides
    std::vector<float> allCentroids(size * numCols);

    MPI_Allgather(localCentroid.values.data(), numCols, MPI_FLOAT, allCentroids.data(), numCols, MPI_FLOAT, MPI_COMM_WORLD);

    // Paso 3: reconstruir centroides globales
    std::vector<Point> globalCentroids(size);

    for (int i = 0; i < size; ++i) {
        globalCentroids[i].values.resize(numCols);

        for (uint32_t j = 0; j < numCols; ++j) {
            globalCentroids[i].values[j] = allCentroids[i * numCols + j];
        }
    }

    // Paso 4: calcular distancia de cada punto a todos los centroides
    std::vector<int> labels(localData.size());

    for (size_t i = 0; i < localData.size(); ++i) {
        float minDist = std::numeric_limits<float>::max();
        int best = 0;

        for (int j = 0; j < size; ++j) {
            float d = distance(localData[i], globalCentroids[j]);

            if (d < minDist) {
                minDist = d;
                best = j;
            }
        }

        labels[i] = best;
    }

    // Paso 5: contar cuántos puntos han cambiado de nodo
    int displaced = 0;

    for (int i = 0; i < labels.size(); ++i) {
        if (labels[i] != rank) {
            displaced++;
        }
    }

    int totalDisplaced;

    MPI_Allreduce(&displaced, &totalDisplaced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Total puntos desplazados en la iteracion inicial: " << totalDisplaced << std::endl;
    }

    MPI_Finalize();

    return 0;
}