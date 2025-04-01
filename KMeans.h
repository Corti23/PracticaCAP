#ifndef KMEANS_H
#define KMEANS_H

#include "Point.h"
#include <vector>
#include <string>

class KMeans {
public:
    KMeans(int k);
    void run(std::vector<Point>& data);
    
private:
    int k;
    std::vector<Point> centroids;

    std::vector<Point> initializeCentroids(const std::vector<Point>& data);
    std::vector<int> assignClusters(const std::vector<Point>& data);
    std::vector<Point> updateCentroids(const std::vector<Point>& data, const std::vector<int>& labels);
};

#endif // KMEANS_H