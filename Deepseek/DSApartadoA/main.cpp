#include "KMeans.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_datos> <k_clusters>" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    int k = std::stoi(argv[2]);
    
    KMeans::run(input_file, k);
    
    return 0;
}