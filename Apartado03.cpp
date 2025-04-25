#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <string>


void leerParteDelArchivo(const std::string& filename, std::vector<std::vector<float>>& data, uint32_t& num_cols, int rank, int size) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Proceso " << rank << ": No se pudo abrir el archivo " << filename << std::endl;

        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    uint32_t num_rows;
    file.read(reinterpret_cast<char*>(&num_rows), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&num_cols), sizeof(uint32_t));

    uint32_t filas_por_proceso = num_rows / size;
    uint32_t resto = num_rows % size;

    uint32_t inicio = rank * filas_por_proceso + std::min(static_cast<uint32_t>(rank), resto);
    uint32_t cantidad = filas_por_proceso + (rank < resto ? 1 : 0);

    file.seekg(sizeof(float) * num_cols * inicio, std::ios::cur);

    data.resize(cantidad, std::vector<float>(num_cols));

    for (uint32_t i = 0; i < cantidad; ++i) {
        for (uint32_t j = 0; j < num_cols; ++j) {
            file.read(reinterpret_cast<char*>(&data[i][j]), sizeof(float));
        }
    }
}

void inicializarGrupos(std::vector<uint32_t>& grupos, uint32_t num_puntos, uint32_t k) {
    grupos.resize(num_puntos);

    for (uint32_t i = 0; i < num_puntos; ++i) {
        grupos[i] = i % k;
    }
}

float distanciaEuclidea(const std::vector<float>& a, const std::vector<float>& b) {
    float suma = 0.0f;

    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        suma += diff * diff;
    }

    return std::sqrt(suma);
}

void calcularCentroideLocal(const std::vector<std::vector<float>>& data, const std::vector<uint32_t>& grupos, std::vector<std::vector<float>>& centroides_locales, std::vector<uint32_t>& contador_local, uint32_t k, uint32_t num_cols) {
    centroides_locales.assign(k, std::vector<float>(num_cols, 0.0f));
    contador_local.assign(k, 0);

    for (size_t i = 0; i < data.size(); ++i) {
        uint32_t grupo = grupos[i];
        contador_local[grupo]++;

        for (uint32_t j = 0; j < num_cols; ++j) {
            centroides_locales[grupo][j] += data[i][j];
        }
    }
}

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == 0) {
            std::cerr << "Uso: mpiexec -n <procesos> ./kmeans_mpi <archivo_binario> <k grupos>" << std::endl;
        }

        MPI_Finalize();

        return 1;
    }

    std::string filename = argv[1];
    uint32_t k = std::stoi(argv[2]);

    // Comenzar medición del tiempo
    double tiempo_inicio = MPI_Wtime();

    std::vector<std::vector<float>> data;
    uint32_t num_cols = 0;
    leerParteDelArchivo(filename, data, num_cols, rank, size);

    std::vector<uint32_t> grupos;
    inicializarGrupos(grupos, data.size(), k);

    std::vector<std::vector<float>> centroides_locales, centroides_globales(k, std::vector<float>(num_cols, 0.0f));
    std::vector<uint32_t> contador_local, contador_global(k, 0);

    int iteraciones = 0;
    const int MAX_ITER = 2000;
    int cambios_globales = 0;
    const float UMBRAL = 0.05f;

    do {
        calcularCentroideLocal(data, grupos, centroides_locales, contador_local, k, num_cols);

        for (uint32_t grupo = 0; grupo < k; ++grupo) {
            MPI_Reduce(centroides_locales[grupo].data(), centroides_globales[grupo].data(), num_cols, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
            MPI_Reduce(&contador_local[grupo], &contador_global[grupo], 1, MPI_UNSIGNED, MPI_SUM, 0, MPI_COMM_WORLD);
        }

        if (rank == 0) {
            for (uint32_t grupo = 0; grupo < k; ++grupo) {
                if (contador_global[grupo] > 0) {
                    for (uint32_t j = 0; j < num_cols; ++j) {
                        centroides_globales[grupo][j] /= contador_global[grupo];
                    }
                }
            }
        }

        for (uint32_t grupo = 0; grupo < k; ++grupo) {
            MPI_Bcast(centroides_globales[grupo].data(), num_cols, MPI_FLOAT, 0, MPI_COMM_WORLD);
        }

        int cambios_locales = 0;

        for (size_t i = 0; i < data.size(); ++i) {
            uint32_t grupo_actual = grupos[i];
            float mejor_distancia = distanciaEuclidea(data[i], centroides_globales[grupo_actual]);
            uint32_t mejor_grupo = grupo_actual;

            for (uint32_t grupo = 0; grupo < k; ++grupo) {
                float d = distanciaEuclidea(data[i], centroides_globales[grupo]);

                if (d < mejor_distancia) {
                    mejor_distancia = d;
                    mejor_grupo = grupo;
                }
            }

            if (mejor_grupo != grupo_actual) {
                grupos[i] = mejor_grupo;
                cambios_locales++;
            }
        }

        MPI_Reduce(&cambios_locales, &cambios_globales, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            float porcentaje = static_cast<float>(cambios_globales) / static_cast<float>(data.size() * size);
            std::cout << "Iteracion " << iteraciones << " - Cambios: " << cambios_globales << " (" << porcentaje * 100.0f << "%)" << std::endl;

            if (porcentaje < UMBRAL) {
                cambios_globales = 0;
            }
        }

        MPI_Bcast(&cambios_globales, 1, MPI_INT, 0, MPI_COMM_WORLD);

        iteraciones++;
    } while (cambios_globales > 0 && iteraciones < MAX_ITER);

    double tiempo_fin = MPI_Wtime();

    if (rank == 0) {
        std::cout << "\nCentroides finales:" << std::endl;

        for (uint32_t grupo = 0; grupo < k; ++grupo) {
            std::cout << "Grupo " << grupo << " -> ";

            for (uint32_t j = 0; j < num_cols; ++j) {
                std::cout << centroides_globales[grupo][j] << " ";
            }

            std::cout << std::endl;
        }

        std::cout << "\nTiempo total de ejecucion: " << (tiempo_fin - tiempo_inicio) << " segundos." << std::endl;
    }

    MPI_Finalize();

    return 0;
}