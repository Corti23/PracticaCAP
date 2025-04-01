#ifndef KMEANS_H
#define KMEANS_H

#include <vector>
#include "point2D.h"

struct Cluster {
    point2D centroid; // Centroid del cluster
    int count; // Número de puntos en el cluster
};

void read_data(const char *filename, std::vector<point2D> &points);
float euclidean_distance(const point2D &a, const point2D &b);
void kmeans(std::vector<point2D> &points, int k, int max_iter);
void print_results(const std::vector<point2D> &points);

#endif // KMEANS_H