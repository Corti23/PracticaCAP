#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "Point.h"
#include <vector>
#include <string>

class DataHandler {
public:
    static std::vector<Point> readBinaryData(const std::string& filename);
};

#endif