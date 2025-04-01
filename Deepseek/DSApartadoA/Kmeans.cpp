#include "KMeans.h"
#include <iostream>
#include <algorithm>

void KMeans::run(const std::string& input_file, int k) {
    int n_rows, n_cols;
    std::vector<Point> data = DataReader::read_binary_data(input_file, n_rows, n_cols);
    std::vector<Cluster> clusters(k, Cluster(n_cols));
    
    // 1. Inicialización
    initialize_clusters(clusters, data, k);
    
    // 2. Bucle principal
    const int max_iterations = 2000;
    const float convergence_threshold = 0.05f * n_rows;
    int iteration = 0;
    int points_moved = 0;
    
    do {
        points_moved = assign_points(clusters, data);
        
        // Actualizar centroides
        for (auto& cluster : clusters) {
            cluster.update_centroid();
        }
        
        iteration++;
        std::cout << "Iteration " << iteration << ": " << points_moved << " points moved" << std::endl;
        
    } while (points_moved > convergence_threshold && iteration < max_iterations);
    
    // 3. Mostrar resultados
    print_results(data, clusters);
}

void KMeans::initialize_clusters(std::vector<Cluster>& clusters, std::vector<Point>& data, int k) {
    // Asignación inicial equitativa por índices
    for (size_t i = 0; i < data.size(); ++i) {
        int cluster_id = i % k;
        data[i].cluster_id = cluster_id;
        clusters[cluster_id].add_point(data[i]);
    }
    
    // Calcular centroides iniciales
    for (auto& cluster : clusters) {
        cluster.update_centroid();
    }
}

int KMeans::assign_points(std::vector<Cluster>& clusters, std::vector<Point>& data) {
    int points_moved = 0;
    
    for (auto& point : data) {
        float min_dist = std::numeric_limits<float>::max();
        int best_cluster = -1;
        
        for (size_t i = 0; i < clusters.size(); ++i) {
            Point centroid(clusters[i].centroid);
            float dist = point.squared_distance(centroid);
            if (dist < min_dist) {
                min_dist = dist;
                best_cluster = i;
            }
        }
        
        if (point.cluster_id != best_cluster) {
            // Eliminar punto del cluster anterior
            auto& old_cluster = clusters[point.cluster_id].points;
            old_cluster.erase(std::remove_if(old_cluster.begin(), old_cluster.end(),
                [&point](const Point& p) { return &p == &point; }), old_cluster.end());
            
            // Añadir al nuevo cluster
            point.cluster_id = best_cluster;
            clusters[best_cluster].add_point(point);
            
            points_moved++;
        }
    }
    
    return points_moved;
}

void KMeans::print_results(const std::vector<Point>& data, const std::vector<Cluster>& clusters) {
    // Calcular estadísticas
    std::vector<float> min_values, max_values, means, variances;
    Statistics::compute(data, min_values, max_values, means, variances);
    
    // Mostrar estadísticas por columna
    std::cout << "\nFinal Statistics:\n";
    for (size_t j = 0; j < min_values.size(); ++j) {
        std::cout << "Column " << j << ":\n";
        std::cout << "  Min: " << min_values[j] << "\n";
        std::cout << "  Max: " << max_values[j] << "\n";
        std::cout << "  Mean: " << means[j] << "\n";
        std::cout << "  Variance: " << variances[j] << "\n\n";
    }
    
    // Mostrar distancia cuadrática media por cluster
    std::cout << "Cluster mean squared distances:\n";
    for (size_t i = 0; i < clusters.size(); ++i) {
        float sum_sq_dist = 0.0f;
        for (const auto& point : clusters[i].points) {
            Point centroid(clusters[i].centroid);
            sum_sq_dist += point.squared_distance(centroid);
        }
        
        float mean_sq_dist = clusters[i].points.empty() ? 0.0f : sum_sq_dist / clusters[i].points.size();
        std::cout << "Cluster " << i << ": " << mean_sq_dist 
                  << " (size: " << clusters[i].points.size() << ")\n";
    }
}