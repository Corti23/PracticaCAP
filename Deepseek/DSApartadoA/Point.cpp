#include "Point.h"
#include <cmath>
#include <stddef.h>

Point::Point(int dimensions) : coordinates(dimensions, 0.0f), cluster_id(-1) {}

float Point::squared_distance(const Point& other) const {
    float dist = 0.0f;
    for (size_t i = 0; i < coordinates.size(); ++i) {
        float diff = coordinates[i] - other.coordinates[i];
        dist += diff * diff;
    }
    return dist;
}

size_t Point::dimensions() const {
    return coordinates.size();
}