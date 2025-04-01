#include "DataHandler.h"
#include "KMeans.h"
#include <iostream>

int main() {
    std::string filename = "salida.bin";
    std::vector<Point> data = DataHandler::readBinaryData(filename);

    if (data.empty()) {
        return 1;
    }
    
    int k = 2;
    
    KMeans kmeans(k);
    kmeans.fit(data);
    
    return 0;
}