#include "kmeans.h"
#include <iostream>
#include <vector>

int main() {
    std::vector<point2D> points;
    const char *filename = "salida.bin"; // Asegúrate de que el archivo generado se llame "salida.bin"

    read_data(filename, points);

    int k = 3; // Número de clusters
    int max_iter = 100; // Número máximo de iteraciones

    kmeans(points, k, max_iter);
    print_results(points);

    return 0;
}