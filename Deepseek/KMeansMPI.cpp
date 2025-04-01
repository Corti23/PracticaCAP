#include "KMeansMPI.h"
#include "DataReader.h"
#include "Statistics.h"
#include <iostream>
#include <limits>
#include <algorithm>

#define MAX_ITERATIONS 2000
#define CONVERGENCE_THRESHOLD 0.05f

void KMeansMPI::run(int argc, char** argv, const std::string& input_file) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int n_rows, n_cols;
    std::vector<Point> all_data;
    
    if (world_rank == 0) {
        all_data = DataReader::read_binary_data(input_file, n_rows, n_cols);
    }

    MPI_Bcast(&n_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<Point> local_data;
    distribute_data(world_rank, world_size, all_data, local_data);

    std::vector<Cluster> clusters(world_size, Cluster(n_cols));
    initialize_clusters(world_rank, world_size, clusters, local_data);

    int iteration = 0;
    int global_changed = 1;

    while (iteration < MAX_ITERATIONS && global_changed > CONVERGENCE_THRESHOLD * n_rows) {
        int local_changed = 0;
        
        assign_points(world_rank, world_size, local_data, clusters, local_changed);
        exchange_points(world_rank, world_size, local_data, clusters);
        
        clusters[world_rank].update_centroid();
        for (int i = 0; i < world_size; ++i) {
            MPI_Bcast(clusters[i].centroid.data(), n_cols, MPI_FLOAT, i, MPI_COMM_WORLD);
        }

        MPI_Allreduce(&local_changed, &global_changed, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        iteration++;

        if (world_rank == 0) {
            std::cout << "Iteration " << iteration << ": " << global_changed << " points changed clusters" << std::endl;
        }
    }

    compute_statistics(world_rank, world_size, local_data, clusters);
    MPI_Finalize();
}

// Implementación de los demás métodos privados...
// (Los métodos distribute_data, initialize_clusters, assign_points, 
// exchange_points y compute_statistics se implementarían aquí siguiendo
// la lógica del código original, pero adaptada al diseño orientado a objetos)