#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include "Point.h"

class Statistics {
public:
    static void compute(const std::vector<Point>& data,
                       std::vector<float>& min_values,
                       std::vector<float>& max_values,
                       std::vector<float>& means,
                       std::vector<float>& variances);
};

#endif // STATISTICS_H