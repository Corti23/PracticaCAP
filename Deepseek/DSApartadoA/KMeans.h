#ifndef KMEANS_H
#define KMEANS_H

#include "Cluster.h"
#include "DataReader.h"
#include "Statistics.h"
#include <string>
#include <vector>

class KMeans {
public:
    static void run(const std::string& input_file, int k);
    
private:
    static void initialize_clusters(std::vector<Cluster>& clusters, std::vector<Point>& data, int k);
    static int assign_points(std::vector<Cluster>& clusters, std::vector<Point>& data);
    static void print_results(const std::vector<Point>& data, const std::vector<Cluster>& clusters);
};

#endif // KMEANS_H