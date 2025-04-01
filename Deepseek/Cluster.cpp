#include "Cluster.h"
#include <numeric>
#include <stddef.h>

Cluster::Cluster() {}

Cluster::Cluster(int dimensions) : centroid(dimensions, 0.0f) {}

void Cluster::update_centroid() {
    if (points.empty()) return;

    std::vector<float> sum(centroid.size(), 0.0f);
    
    #pragma omp parallel for
    for (size_t i = 0; i < points.size(); ++i) {
        #pragma omp simd
        for (size_t j = 0; j < centroid.size(); ++j) {
            #pragma omp atomic
            sum[j] += points[i].coordinates[j];
        }
    }

    #pragma omp simd
    for (size_t j = 0; j < centroid.size(); ++j) {
        centroid[j] = sum[j] / points.size();
    }
}

void Cluster::clear_points() {
    points.clear();
}

void Cluster::add_point(const Point& point) {
    points.push_back(point);
}