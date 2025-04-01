#ifndef KMEANS_H
#define KMEANS_H

#include <vector>
#include "random_generator.h"

struct Cluster {
    point2D centroid; // Cambiar a point2D
    int count;
};

void read_data(const char *filename, std::vector<point2D> &points);
float euclidean_distance(const point2D &a, const point2D &b);
void kmeans(std::vector<point2D> &points, int k, int max_iter);
void print_results(const std::vector<point2D> &points);
void calculate_statistics(const std::vector<point2D> &points);

#endif // KMEANS_H