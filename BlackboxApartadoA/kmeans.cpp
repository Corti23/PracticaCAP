#include "kmeans.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>

void read_data(const char *filename, std::vector<point2D> &points) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return;
    }

    int num_points;
    file.read(reinterpret_cast<char*>(&num_points), sizeof(int));
    points.resize(num_points);
    file.read(reinterpret_cast<char*>(points.data()), num_points * sizeof(point2D));
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
        for (auto &point : points) {
            float min_dist = std::numeric_limits<float>::max();
            int best_cluster = -1;
            for (int j = 0; j < k; j++) {
                float dist = euclidean_distance(point, clusters[j].centroid);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_cluster = j;
                }
            }
            point.cluster = best_cluster; // Asignar el cluster
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

    // Calcular la distancia cuadrática media
    for (int j = 0; j < k; j++) {
        float sum_squared_distances = 0.0f;
        for (const auto &point : points) {
            if (point.cluster == j) {
                float dist = euclidean_distance(point, clusters[j].centroid);
                sum_squared_distances += dist * dist;
            }
        }
        float mean_squared_distance = sum_squared_distances / clusters[j].count;
        std::cout << "Cluster " << j << ": Distancia cuadrática media = " << mean_squared_distance << std::endl;
    }
}

void print_results(const std::vector<point2D> &points) {
    for (const auto &point : points) {
        std::cout << "Point (" << point.x << ", " << point.y << ") assigned to cluster " << point.cluster << std::endl;
    }
}