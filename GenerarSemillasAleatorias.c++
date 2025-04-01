#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>

#define PI 3.14159265358979323846f

struct point2D {
    float x;
    float y;
};

point2D getRandomPoint(float x0, float y0, float maxRadius, float minRadius = 0.0f) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distRadius(minRadius, maxRadius);
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * PI);

    float r = distRadius(gen);
    float alpha = distAngle(gen);

    return {x0 + r * cos(alpha), y0 + r * sin(alpha)};
}

int main() {
    int nClusters = 1;
    int nPointsPerCluster = 50;
    std::vector<point2D> data;

    for (int i = 0; i < nClusters; i++) {
        point2D centroid = getRandomPoint(0.0f, 0.0f, 20.0f, 0.0f);

        for (int j = 0; j < nPointsPerCluster; j++) {
            data.push_back(getRandomPoint(centroid.x, centroid.y, 1.0f));
        }
    }

    FILE* resultsFile = fopen("salida.bin", "wb");

    if (!resultsFile) {
        std::cerr << "Error al abrir el archivo para escritura." << std::endl;

        return 1;
    }

    int nFilas = nClusters * nPointsPerCluster;
    int nCol = 2;

    fwrite(&nFilas, sizeof(int), 1, resultsFile);
    fwrite(&nCol, sizeof(int), 1, resultsFile);
    fwrite(data.data(), sizeof(point2D), data.size(), resultsFile);

    fclose(resultsFile);

    for (const auto& p : data) {
        std::cout << p.x << "\t" << p.y << "\n";
    }

    return 0;
}