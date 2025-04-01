#include "Cluster.h"
#include <numeric>
#include <stddef.h>

Cluster::Cluster(int dimensions) : centroid(dimensions, 0.0f) {}

void Cluster::update_centroid() {
    if (points.empty()) return;
    
    std::fill(centroid.begin(), centroid.end(), 0.0f);
    for (const auto& point : points) {
        for (size_t i = 0; i < centroid.size(); ++i) {
            centroid[i] += point.coordinates[i];
        }
    }
    
    for (size_t i = 0; i < centroid.size(); ++i) {
        centroid[i] /= points.size();
    }
}

void Cluster::clear_points() {
    points.clear();
}

void Cluster::add_point(const Point& point) {
    points.push_back(point);
}