#include <iostream>
#include <iomanip>
#include <string>

#include "grafoADT.h"
#include "redUtilidades.h"
#include "medidasCentralidad.h"
#include "rendimientoCentralidad.h"


int main() {
    // ==========================================
    // INSTANCIACIÓN DE LAS REDES
    // ==========================================
    Grafo<std::string, AristaData> redActores(false);       // false = no dirigido
    Grafo<std::string, AristaData> redIoT(true);            // true = dirigido
    Grafo<std::string, AristaData> redNetScience(false);    // false = no dirigido

    // ==========================================
    // CARGAR DATASETS
    // ==========================================
    std::cout << "\nCargando datasets...\n";

    // Selección de rutas para Windows y Linux
    #ifdef _WIN32
        std::string ruta_Actores = "..\\datasets\\imdb_edgelist.csv";
        std::string ruta_IoT = "..\\datasets\\train_test_network.csv";
        std::string ruta_NetScience = "..\\datasets\\NetScience.csv";
    #else
        // Sin el "../"
        std::string ruta_Actores = "datasets/imdb_edgelist.csv";
        std::string ruta_IoT = "datasets/train_test_network.csv";
        std::string ruta_NetScience = "datasets/NetScience.csv";
    #endif

    auto inicio_actores = std::chrono::high_resolution_clock::now();
    RedUtilidades::cargarDatasetCSV(ruta_Actores, redActores, "From", "To", "Strength");
    auto fin_actores = std::chrono::high_resolution_clock::now();

    auto inicio_IoT = std::chrono::high_resolution_clock::now();
    RedUtilidades::cargarDatasetCSV(ruta_IoT, redIoT, "src_ip", "dst_ip", "duration");
    auto fin_IoT = std::chrono::high_resolution_clock::now();

    auto inicio_NetScience = std::chrono::high_resolution_clock::now();
    RedUtilidades::cargarDatasetCSV(ruta_NetScience, redNetScience, "Source", "Target", "Weight");
    auto fin_NetScience = std::chrono::high_resolution_clock::now();

    double memoriaActoresMB = redActores.estimarMemoriaBytes() / (1024.0 * 1024.0);
    double memoriaIoTMB = redIoT.estimarMemoriaBytes() / (1024.0 * 1024.0);
    double memoriaNetScienceMB = redNetScience.estimarMemoriaBytes() / (1024.0 * 1024.0);

    std::cout << "========================================\n";
    std::cout << "Datasets cargados exitosamente.\n";
    std::cout << "========================================\n";

    std::cout << "Red de Actores: " << redActores.numVertices() << " nodos, " << redActores.edges().size() << " conexiones.\n" 
              << "Tiempo de carga: " << std::chrono::duration<double, std::milli>(fin_actores - inicio_actores).count() << " ms\n"
              << "Espacio en memoria: " << std::fixed << std::setprecision(2) << memoriaActoresMB << " MB\n\n";

    std::cout << "Red de IoT: " << redIoT.numVertices() << " nodos, " << redIoT.edges().size() << " conexiones.\n"
              << "Tiempo de carga: " << std::chrono::duration<double, std::milli>(fin_IoT - inicio_IoT).count() << " ms\n"
              << "Espacio en memoria: " << std::fixed << std::setprecision(2) << memoriaIoTMB << " MB\n\n";

    std::cout << "Red NetScience: " << redNetScience.numVertices() << " nodos, " << redNetScience.edges().size() << " conexiones.\n"
              << "Tiempo de carga: " << std::chrono::duration<double, std::milli>(fin_NetScience - inicio_NetScience).count() << " ms\n"
              << "Espacio en memoria: " << std::fixed << std::setprecision(2) << memoriaNetScienceMB << " MB\n";
    std::cout << "========================================\n";
   
    // ==========================================
    // MEDIDAS Y RENDIMIENTO
    // ==========================================

    // Degree Centrality - Red IoT
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularDegreeCentrality(G);
    }, "Degree Centrality - Red IoT");

    // Betweenness Centrality - Red IoT (requiere extractor de peso)
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularBetweennessCentrality(G, AristaData::getPesoTotal);
    }, "Betweenness Centrality - Red IoT");

    // Closeness Centrality - Red IoT 
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularClosenessCentrality(G);
    }, "Closeness Centrality - Red IoT");

    // PageRank - Red IoT
    auto resPR = AnalizadorCentralidad<std::string, AristaData>::calcularPageRank(redIoT);
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularPageRank(G);
    }, "PageRank - Red de IoT");

    // Average Shortest Path - Red IoT
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularAverageShortestPath(G);
    }, "Average Shortest Path - Red IoT");

    // Clustering Local (Cálculo para todos los nodos) - Red IoT
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        for (int v : G.vertices()) {
            AnalizadorCentralidad<std::string, AristaData>::calcularClustering(G, v);
        }
    }, "Clustering Local (Todos los nodos) - Red IoT");

    // Eigenvector Centrality - Red IoT
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularEigenvectorCentrality(G);
    }, "Eigenvector Centrality - Red IoT");

    return 0;
}