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