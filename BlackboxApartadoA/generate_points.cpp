#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>  // Para fopen

#define PI 3.141582f

struct point2D {
    float x;
    float y;
};

point2D getRandomPoint(float x0, float y0, float maxRadius, float minRadius = 0.0f) {
    point2D p;
    float r = minRadius + (maxRadius - minRadius) * (float)rand() / RAND_MAX;
    float alpha = 2.0f * PI * (float)rand() / RAND_MAX;
    p.x = x0 + r * cos(alpha);
    p.y = y0 + r * sin(alpha);
    return p;
}

int main() {
    int nClusters = 1; // Número de clusters
    int nPointsPerCluster = 50; // Puntos por cluster

    std::vector<point2D> data;
    for (int i = 0; i < nClusters; i++) {
        point2D centroid = getRandomPoint(0.0f, 0.0f, 20.0f, 0.0f);
        for (int j = 0; j < nPointsPerCluster; j++)
            data.push_back(getRandomPoint(centroid.x, centroid.y, 1.0f));
    }

    FILE* resultsFile = fopen("salida.bin", "wb"); // Guardar en "salida.bin"
    if (!resultsFile) {
        std::cerr << "Error al abrir el archivo para escritura." << std::endl;
        return 1; // Salir si hay un error
    }

    int nFilas = nClusters * nPointsPerCluster;
    int nCol = 2;
    fwrite(&nFilas, sizeof(int), 1, resultsFile);
    fwrite(&nCol, sizeof(int), 1, resultsFile);
    fwrite(data.data(), sizeof(point2D), data.size(), resultsFile); // Guardar los puntos
    fclose(resultsFile);

    for (const auto &point : data)
        std::cout << point.x << "\t" << point.y << "\n";

    return 0;
}