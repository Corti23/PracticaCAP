#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include "Point.h"
#include <vector>
#include <string>

class DataGenerator {
public:
    static void generateData(const std::string& filename, int nClusters, int nPointsPerCluster);
    static std::vector<Point> readBinaryData(const std::string& filename);
};

#endif // DATA_GENERATOR_H
