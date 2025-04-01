#ifndef POINT_H
#define POINT_H

struct Point {
    float x, y;
    bool operator==(const Point& other) const;
};

float distance(const Point& a, const Point& b);

#endif