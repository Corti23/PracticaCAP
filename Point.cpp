#include "Point.h"
#include <cmath>

bool Point::operator==(const Point& other) const {
    return (x == other.x) && (y == other.y);
}

float distance(const Point& a, const Point& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}