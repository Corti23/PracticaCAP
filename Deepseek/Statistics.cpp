#include "Statistics.h"
#include <algorithm>
#include <numeric>
#include <cmath>

void Statistics::compute(const std::vector<std::vector<float>>& data_by_column,
                        std::vector<float>& min_values,
                        std::vector<float>& max_values,
                        std::vector<float>& means,
                        std::vector<float>& variances) {
    int n_cols = static_cast<int>(data_by_column.size());
    
    min_values.resize(n_cols);
    max_values.resize(n_cols);
    means.resize(n_cols);
    variances.resize(n_cols);

    #pragma omp parallel for
    for (int j = 0; j < n_cols; ++j) {
        const auto& column = data_by_column[j];
        if (column.empty()) continue;

        // Min and max
        auto minmax = std::minmax_element(column.begin(), column.end());
        min_values[j] = *minmax.first;
        max_values[j] = *minmax.second;

        // Mean
        float sum = std::accumulate(column.begin(), column.end(), 0.0f);
        means[j] = sum / column.size();

        // Variance
        float sq_sum = 0.0f;
        #pragma omp simd reduction(+:sq_sum)
        for (size_t i = 0; i < column.size(); ++i) {
            float diff = column[i] - means[j];
            sq_sum += diff * diff;
        }
        variances[j] = sq_sum / column.size();
    }
}