#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <omp.h>

// Leer archivo binario
bool leerDatosBinarios(const std::string& filename, std::vector<std::vector<float>>& data, uint32_t& num_cols) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;

        return false;
    }

    uint32_t num_rows;
    file.read(reinterpret_cast<char*>(&num_rows), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));

    data.resize(num_rows, std::vector<float>(num_cols));

    for (uint32_t i = 0; i < num_rows; ++i) {
        for (uint32_t j = 0; j < num_cols; ++j) {
            file.read(reinterpret_cast<char*>(&data[i][j]), sizeof(float));
        }
    }
    std::cout << "Leidos " << num_rows << " registros con " << num_cols << " columnas.\n";

    return true;
}

// Distancia euclídea
float calcularDistancia(const std::vector<float>& a, const std::vector<float>& b) {
    float dist = 0.0f;

    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        dist += diff * diff;
    }

    return std::sqrt(dist);
}

// Cálculo de centroides (paralelizado)
std::vector<float> calcularCentroide(const std::vector<std::vector<float>>& data, const std::vector<uint32_t>& indices, uint32_t num_cols) {
    std::vector<float> centroide(num_cols, 0.0f);

    if (indices.empty()) {
        return centroide;
    }

    #pragma omp parallel for
    for (uint32_t j = 0; j < num_cols; ++j) {
        for (uint32_t idx : indices) {
            centroide[j] += data[idx][j];
        }

        centroide[j] /= indices.size();
    }

    return centroide;
}

// Estadísticas (media, min, max, varianza) paralelizadas
void calcularEstadisticas(const std::vector<std::vector<float>>& data, const std::vector<uint32_t>& indices, uint32_t num_cols, uint32_t grupo_id) {
    if (indices.empty()) {
        std::cout << "Grupo " << grupo_id << " esta vacio.\n";

        return;
    }

    std::vector<float> media(num_cols, 0.0f);
    std::vector<float> minimo(num_cols, std::numeric_limits<float>::max());
    std::vector<float> maximo(num_cols, std::numeric_limits<float>::lowest());
    std::vector<float> varianza(num_cols, 0.0f);

    #pragma omp parallel for
    for (uint32_t j = 0; j < num_cols; ++j) {
        for (uint32_t idx : indices) {
            float value = data[idx][j];
            media[j] += value;
            minimo[j] = std::min(minimo[j], value);
            maximo[j] = std::max(maximo[j], value);
        }

        media[j] /= indices.size();
    }

    #pragma omp parallel for
    for (uint32_t j = 0; j < num_cols; ++j) {
        for (uint32_t idx : indices) {
            float diff = data[idx][j] - media[j];
            varianza[j] += diff * diff;
        }

        varianza[j] /= indices.size();
    }

    std::cout << "\nEstadisticas del Grupo " << grupo_id << ":\n";

    for (uint32_t j = 0; j < num_cols; ++j) {
        std::cout << " - Columna " << j << ":\n"
                  << "      Media: " << media[j] << "\n"
                  << "      Min: " << minimo[j] << "\n"
                  << "      Max: " << maximo[j] << "\n"
                  << "      Var: " << varianza[j] << std::endl;
    }
}

// Algoritmo K-Means (paralelización aplicada)
void kMeans(const std::vector<std::vector<float>>& data, uint32_t num_cols, uint32_t k = 4) {
    uint32_t num_rows = data.size();

    std::vector<std::vector<uint32_t>> grupos(k);

    // Inicialización equitativa
    for (uint32_t i = 0; i < num_rows; ++i) {
        grupos[i % k].push_back(i);
    }

    std::vector<std::vector<float>> centroides(k);

    for (uint32_t g = 0; g < k; ++g) {
        centroides[g] = calcularCentroide(data, grupos[g], num_cols);
    }

    bool continuar = true;
    uint32_t iteraciones = 0;

    while (continuar && iteraciones < 2000) {
        continuar = false;
        iteraciones++;

        uint32_t puntos_movidos = 0;

        std::vector<std::vector<uint32_t>> nuevosGrupos(k);

        // Paralelizamos el for de asignación de puntos
        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(num_rows); ++i) {
            float min_dist = std::numeric_limits<float>::max();
            uint32_t mejor_grupo = 0;

            for (uint32_t g = 0; g < k; ++g) {
                float dist = calcularDistancia(data[i], centroides[g]);

                if (dist < min_dist) {
                    min_dist = dist;
                    mejor_grupo = g;
                }
            }

            // Protección al escribir en grupos compartidos
            #pragma omp critical
            {
                nuevosGrupos[mejor_grupo].push_back(i);
            }
        }

        for (uint32_t g = 0; g < k; ++g) {
            std::sort(grupos[g].begin(), grupos[g].end());
            std::sort(nuevosGrupos[g].begin(), nuevosGrupos[g].end());

            if (grupos[g] != nuevosGrupos[g]) {
                puntos_movidos += std::abs((int)grupos[g].size() - (int)nuevosGrupos[g].size());
                grupos[g] = nuevosGrupos[g];
                continuar = true;
            }
        }

        for (uint32_t g = 0; g < k; ++g) {
            centroides[g] = calcularCentroide(data, grupos[g], num_cols);
        }

        if ((float)puntos_movidos / num_rows < 0.05f) {
            continuar = false;
        }
    }

    std::cout << "\nIteraciones realizadas: " << iteraciones << std::endl;

    for (uint32_t g = 0; g < k; ++g) {
        std::cout << "\nGrupo " << g << " contiene " << grupos[g].size() << " puntos.\n";
        calcularEstadisticas(data, grupos[g], num_cols, g);
    }
}

// Main
int main() {
    std::string archivo = "datos.bin";
    std::vector<std::vector<float>> data;

    uint32_t num_cols = 0;

    if (leerDatosBinarios(archivo, data, num_cols)) {
        auto tiempoInicio = std::chrono::high_resolution_clock::now();
        kMeans(data, num_cols, 4); // k = 4
        auto tiempoFin = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> tiempoTotal = tiempoFin - tiempoInicio;

        std::cout << "Tiempo de ejecucion: " << tiempoTotal.count() << "s\n";
    }

    return 0;
}