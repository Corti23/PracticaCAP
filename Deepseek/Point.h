#ifndef POINT_H
#define POINT_H

#include <vector>

class Point {
public:
    std::vector<float> coordinates;
    int cluster_id;

    Point();
    explicit Point(int dimensions);
    Point(const std::vector<float>& coords, int cluster = -1);
    
    float squared_distance(const Point& other) const;
    int dimensions() const;
};

#endif // POINT_H