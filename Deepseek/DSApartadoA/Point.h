#ifndef POINT_H
#define POINT_H

#include <vector>

class Point {
public:
    std::vector<float> coordinates;
    int cluster_id;
    
    Point(int dimensions = 0);
    float squared_distance(const Point& other) const;
    size_t dimensions() const;
};

#endif // POINT_H