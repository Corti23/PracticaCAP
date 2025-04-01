#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <vector>

struct point2D {
    float x;
    float y;
    int cluster; // Agregar el campo cluster
};

point2D getRandomPoint(float x0, float y0, float maxRadius, float minRadius = 0.0f);
void generateRandomData(int nClusters, int nPointsPerCluster, std::vector<point2D> &data);
void saveDataToFile(const char *filename, const std::vector<point2D> &data);

#endif // RANDOM_GENERATOR_H