#include "kmeans.h"
#include "random_generator.h"
#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<point2D> points;

    // Generar datos aleatorios en el proceso raíz
    if (rank == 0) {
        int nClusters = 5; // Número de clusters
        int nPointsPerCluster = 50; // Puntos por cluster
        generateRandomData(nClusters, nPointsPerCluster, points);
        saveDataToFile("salida.bin", points);
    }

    // Broadcast the number of points and dimensions to all processes
    int num_points = points.size();
    MPI_Bcast(&num_points, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Resize points vector in all processes
    points.resize(num_points);

    // Broadcast points to all processes
    MPI_Bcast(points.data(), num_points * sizeof(point2D), MPI_BYTE, 0, MPI_COMM_WORLD);

    int k = 5; // Número de clusters
    int max_iter = 2000;

    // Ejecutar k-means
    kmeans(points, k, max_iter);

    // Recoger resultados de vuelta al proceso raíz
    MPI_Gather(points.data(), num_points * sizeof(point2D), MPI_BYTE, points.data(), num_points * sizeof(point2D), MPI_BYTE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        print_results(points);
        calculate_statistics(points);
    }

    MPI_Finalize();
    return 0;
}