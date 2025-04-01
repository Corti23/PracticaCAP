#include "Point.h"
#include <cmath>
#include <stddef.h>

Point::Point() : cluster_id(-1) {}

Point::Point(int dimensions) : coordinates(dimensions, 0.0f), cluster_id(-1) {}

Point::Point(const std::vector<float>& coords, int cluster) 
    : coordinates(coords), cluster_id(cluster) {}

float Point::squared_distance(const Point& other) const {
    float dist = 0.0f;
    #pragma omp simd reduction(+:dist)
    for (size_t i = 0; i < coordinates.size(); ++i) {
        float diff = coordinates[i] - other.coordinates[i];
        dist += diff * diff;
    }
    return dist;
}

int Point::dimensions() const {
    return static_cast<int>(coordinates.size());
}