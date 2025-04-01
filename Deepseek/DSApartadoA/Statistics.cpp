#include "Statistics.h"
#include <limits>
#include <numeric>
#include <stddef.h>

void Statistics::compute(const std::vector<Point>& data,
                        std::vector<float>& min_values,
                        std::vector<float>& max_values,
                        std::vector<float>& means,
                        std::vector<float>& variances) {
    if (data.empty()) return;
    
    size_t n_cols = data[0].dimensions();
    min_values.resize(n_cols, std::numeric_limits<float>::max());
    max_values.resize(n_cols, std::numeric_limits<float>::lowest());
    means.resize(n_cols, 0.0f);
    variances.resize(n_cols, 0.0f);

    // Primera pasada: calcular min, max y sumas
    for (const auto& point : data) {
        for (size_t j = 0; j < n_cols; ++j) {
            float val = point.coordinates[j];
            if (val < min_values[j]) min_values[j] = val;
            if (val > max_values[j]) max_values[j] = val;
            means[j] += val;
        }
    }

    // Calcular medias
    for (size_t j = 0; j < n_cols; ++j) {
        means[j] /= data.size();
    }

    // Segunda pasada: calcular varianzas
    for (const auto& point : data) {
        for (size_t j = 0; j < n_cols; ++j) {
            float diff = point.coordinates[j] - means[j];
            variances[j] += diff * diff;
        }
    }

    // Calcular varianzas finales
    for (size_t j = 0; j < n_cols; ++j) {
        variances[j] /= data.size();
    }
}