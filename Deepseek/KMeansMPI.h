#ifndef KMEANS_MPI_H
#define KMEANS_MPI_H

#include "Point.h"
#include "Cluster.h"
#include <vector>
#include <string>
#include <mpi.h>

class KMeansMPI {
public:
    static void run(int argc, char** argv, const std::string& input_file);

private:
    static void distribute_data(int world_rank, int world_size, 
                              const std::vector<Point>& all_data,
                              std::vector<Point>& local_data);
    
    static void initialize_clusters(int world_rank, int world_size,
                                  std::vector<Cluster>& clusters,
                                  const std::vector<Point>& local_data);
    
    static void assign_points(int world_rank, int world_size,
                            std::vector<Point>& local_data,
                            std::vector<Cluster>& clusters,
                            int& local_changed);
    
    static void exchange_points(int world_rank, int world_size,
                              std::vector<Point>& local_data,
                              const std::vector<Cluster>& clusters);
    
    static void compute_statistics(int world_rank, int world_size,
                                 const std::vector<Point>& local_data,
                                 const std::vector<Cluster>& clusters);
};

#endif // KMEANS_MPI_H