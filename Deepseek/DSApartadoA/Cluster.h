#ifndef CLUSTER_H
#define CLUSTER_H

#include "Point.h"
#include <vector>

class Cluster {
public:
    std::vector<float> centroid;
    std::vector<Point> points;
    
    Cluster(int dimensions = 0);
    void update_centroid();
    void clear_points();
    void add_point(const Point& point);
};

#endif // CLUSTER_H