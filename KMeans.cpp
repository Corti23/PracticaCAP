#include "KMeans.h"
#include <random>
#include <limits>
#include <iostream>
#include <chrono>

KMeans::KMeans(int k) : k(k) {}

void KMeans::fit(std::vector<Point>& data) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Point> centroids = initializeCentroids(data);
    std::vector<int> labels;
    std::vector<Point> newCentroids;
    
    for (int i = 0; i < 10; ++i) {
        labels = assignClusters(data, centroids);
        newCentroids = updateCentroids(data, labels);

        if (newCentroids == centroids) {
            break;
        }

        centroids = newCentroids;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "Centroides finales:\n";

    for (const auto& c : centroids) {
        std::cout << c.x << " " << c.y << std::endl;
    }

    std::cout << "Tiempo de ejecución: " << duration.count() << " segundos\n";
}

std::vector<Point> KMeans::initializeCentroids(const std::vector<Point>& data) {
    std::vector<Point> centroids;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, data.size() - 1);

    for (int i = 0; i < k; ++i) {
        centroids.push_back(data[dist(gen)]);
    }

    return centroids;
}

std::vector<int> KMeans::assignClusters(const std::vector<Point>& data, const std::vector<Point>& centroids) {
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

std::vector<Point> KMeans::updateCentroids(const std::vector<Point>& data, const std::vector<int>& labels) {
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