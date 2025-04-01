#include "kmeans.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <omp.h>

void read_data(const char *filename, std::vector<point2D> &points) {
    std::ifstream file(filename, std::ios::binary);
    int num_points, num_dimensions;
    file.read(reinterpret_cast<char*>(&num_points), sizeof(int));
    file.read(reinterpret_cast<char*>(&num_dimensions), sizeof(int));

    points.resize(num_points);
    for (int i = 0; i < num_points; i++) {
        file.read(reinterpret_cast<char*>(&points[i]), sizeof(point2D));
    }
}

float euclidean_distance(const point2D &a, const point2D &b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void kmeans(std::vector<point2D> &points, int k, int max_iter) {
    std::vector<Cluster> clusters(k);
    
    // Inicializar centroides
    for (int i = 0; i < k; i++) {
        clusters[i].centroid = points[i]; // Inicialización simple
        clusters[i].count = 0;
    }

    for (int iter = 0; iter < max_iter; iter++) {
        // Asignar puntos al centroide más cercano
        #pragma omp parallel for
        for (size_t i = 0; i < points.size(); i++) {
            float min_dist = std::numeric_limits<float>::max();
            int best_cluster = -1;
            for (int j = 0; j < k; j++) {
                float dist = euclidean_distance(points[i], clusters[j].centroid);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_cluster = j;
                }
            }
            points[i].cluster = best_cluster; // Asignar el cluster
        }

        // Actualizar centroides
        for (int j = 0; j < k; j++) {
            clusters[j].centroid = {0.0f, 0.0f}; // Reiniciar centroides
            clusters[j].count = 0;
        }

        for (const auto &point : points) {
            int cluster_id = point.cluster;
            clusters[cluster_id].centroid.x += point.x;
            clusters[cluster_id].centroid.y += point.y;
            clusters[cluster_id].count++;
        }

        for (int j = 0; j < k; j++) {
            if (clusters[j].count > 0) {
                clusters[j].centroid.x /= clusters[j].count;
                clusters[j].centroid.y /= clusters[j].count;
            }
        }
    }
}

void print_results(const std::vector<point2D> &points) {
    for (size_t i = 0; i < points.size(); i++) {
        std::cout << "Point (" << points[i].x << ", " << points[i].y << ") assigned to cluster " << points[i].cluster << std::endl;
    }
}

void calculate_statistics(const std::vector<point2D> &points) {
    if (points.empty()) return;

    int num_dimensions = 2; // 2D points
    std::vector<float> min_values(num_dimensions, std::numeric_limits<float>::max());
    std::vector<float> max_values(num_dimensions, std::numeric_limits<float>::lowest());
    std::vector<float> sum_values(num_dimensions, 0.0);
    std::vector<float> sum_squared_values(num_dimensions, 0.0);
    int count = points.size();

    for (const auto &point : points) {
        min_values[0] = std::min(min_values[0], point.x);
        min_values[1] = std::min(min_values[1], point.y);
        max_values[0] = std::max(max_values[0], point.x);
        max_values[1] = std::max(max_values[1], point.y);
        sum_values[0] += point.x;
        sum_values[1] += point.y;
        sum_squared_values[0] += point.x * point.x;
        sum_squared_values[1] += point.y * point.y;
    }

    std::cout << "Statistics:" << std::endl;
    for (int d = 0; d < num_dimensions; d++) {
        float mean = sum_values[d] / count;
        float variance = (sum_squared_values[d] / count) - (mean * mean);
        std::cout << "Dimension " << d << ": Min = " << min_values[d]
                  << ", Max = " << max_values[d]
                  << ", Mean = " << mean
                  << ", Variance = " << variance << std::endl;
    }
}