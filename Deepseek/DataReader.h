#ifndef DATA_READER_H
#define DATA_READER_H

#include "Point.h"
#include <vector>
#include <string>

class DataReader {
public:
    static std::vector<Point> read_binary_data(const std::string& filename, int& n_rows, int& n_cols);
    static void generate_test_data(const std::string& filename, int n_clusters = 4, int points_per_cluster = 250);
};

#endif // DATA_READER_H