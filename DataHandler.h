#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "Point.h"
#include <vector>
#include <string>

class DataHandler {
public:
    static void generateData(const std::string& filename, int nClusters, int nPointsPerCluster);
    static std::vector<Point> readBinaryData(const std::string& filename);
};

#endif // DATAHANDLER_H