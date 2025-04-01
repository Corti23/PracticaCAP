#include "DataGenerator.h"
#include "KMeans.h"
#include <iostream>

int main() {
    std::string filename = "salida.bin";
    DataGenerator::generateData(filename, 2, 50);
    std::vector<Point> data = DataGenerator::readBinaryData(filename);

    if (!data.empty()) {
        KMeans kmeans(2);
        kmeans.run(data);
    }

    return 0;
}
