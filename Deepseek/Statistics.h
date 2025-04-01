#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>

class Statistics {
public:
    static void compute(const std::vector<std::vector<float>>& data_by_column,
                       std::vector<float>& min_values,
                       std::vector<float>& max_values,
                       std::vector<float>& means,
                       std::vector<float>& variances);
};

#endif // STATISTICS_H