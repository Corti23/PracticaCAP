#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <random>

struct Point {
    float x, y;
};

std::vector<Point> generateRandomClusters(int nClusters, int nPointsPerCluster) {
    std::vector<Point> data;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> clusterDist(-20.0, 20.0);
    std::uniform_real_distribution<float> pointDist(-1.0, 1.0);
    
    for (int i = 0; i < nClusters; ++i) {
        float clusterX = clusterDist(gen);
        float clusterY = clusterDist(gen);

        for (int j = 0; j < nPointsPerCluster; ++j) {
            data.push_back({clusterX + pointDist(gen), clusterY + pointDist(gen)});
        }
    }

    return data;
}

float distance(const Point& a, const Point& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

std::vector<Point> initializeCentroids(const std::vector<Point>& data, int k) {
    std::vector<Point> centroids(k);

    for (int i = 0; i < k; ++i) {
        centroids[i] = data[rand() % data.size()];
    }

    return centroids;
}

std::vector<int> assignClusters(const std::vector<Point>& data, const std::vector<Point>& centroids) {
    std::vector<int> labels(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        float minDist = std::numeric_limits<float>::max();
        int bestCluster = 0;

        for (size_t j = 0; j < centroids.size(); ++j) {
            float d = distance(data[i], centroids[j]);

            if (d < minDist) {
                minDist = d;
                bestCluster = j;
            }
        }

        labels[i] = bestCluster;
    }

    return labels;
}

std::vector<Point> updateCentroids(const std::vector<Point>& data, const std::vector<int>& labels, int k) {
    std::vector<Point> newCentroids(k, {0, 0});
    std::vector<int> counts(k, 0);
    
    for (size_t i = 0; i < data.size(); ++i) {
        newCentroids[labels[i]].x += data[i].x;
        newCentroids[labels[i]].y += data[i].y;
        counts[labels[i]]++;
    }
    
    for (int i = 0; i < k; ++i) {
        if (counts[i] > 0) {
            newCentroids[i].x /= counts[i];
            newCentroids[i].y /= counts[i];
        }
    }

    return newCentroids;
}

void kMeans(std::vector<Point>& data, int k, int maxIterations = 100) {
    std::vector<Point> centroids = initializeCentroids(data, k);
    std::vector<int> labels;
    
    for (int iter = 0; iter < maxIterations; ++iter) {
        labels = assignClusters(data, centroids);
        std::vector<Point> newCentroids = updateCentroids(data, labels, k);

        if (centroids == newCentroids) {
            // Converged
            break; 
        } 

        centroids = newCentroids;
    }
    
    std::cout << "Centroides finales:" << std::endl;

    for (const auto& c : centroids) {
        std::cout << c.x << " " << c.y << std::endl;
    }
}

int main() {
    int nClusters = 3, nPointsPerCluster = 100;
    std::vector<Point> data = generateRandomClusters(nClusters, nPointsPerCluster);
    
    int k = nClusters;
    kMeans(data, k);

    return 0;
}