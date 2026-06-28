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
    /*
    // ==========================================
    // BUSCAR EL NODO CON MAYOR PAGERANK
    // ==========================================
    auto resultados_pr = AnalizadorCentralidad<std::string, AristaData>::calcularPageRank(redActores);

    if (!resultados_pr.empty()) {
        int id_max = -1;
        double max_score = -1.0; 

        // Recorremos todos los resultados para buscar el mayor
        for (const auto& par : resultados_pr) {
            int id_actual = par.first;
            double score_actual = par.second;

            if (score_actual > max_score) {
                max_score = score_actual;
                id_max = id_actual;
            }
        }

        // Recuperamos el nombre del nodo usando su ID interno
        std::string nombre_ganador = redActores.getVertexElement(id_max);

        std::cout << "\n========================================\n";
        std::cout << "  EL NODO MAS IMPORTANTE (MAYOR PAGERANK)\n";
        std::cout << "========================================\n";
        std::cout << "Red evaluada: Actores\n";
        std::cout << "Nodo: " << nombre_ganador << "\n";
        std::cout << "Score: " << std::fixed << std::setprecision(4) << max_score << "\n";
        std::cout << "========================================\n";
    } else {
        std::cout << "La red de Actores esta vacia, no hay PageRank que mostrar.\n";
    }

    // ==========================================
    // PRUEBA: MAYOR DEGREE CENTRALITY
    // ==========================================
    auto resultados_dc = AnalizadorCentralidad<std::string, AristaData>::calcularDegreeCentrality(redActores);

    if (!resultados_dc.empty()) {
        int id_max_dc = -1;
        double max_score_dc = -1.0; 

        for (const auto& par : resultados_dc) {
            if (par.second > max_score_dc) {
                max_score_dc = par.second;
                id_max_dc = par.first;
            }
        }

        std::string nombre_ganador_dc = redActores.getVertexElement(id_max_dc);

        std::cout << "\n========================================\n";
        std::cout << " MAYOR DEGREE CENTRALITY (Mas conexiones)\n";
        std::cout << "========================================\n";
        std::cout << "Red evaluada: Actores\n";
        std::cout << "Nodo: " << nombre_ganador_dc << "\n";
        std::cout << "Score: " << std::fixed << std::setprecision(4) << max_score_dc << "\n";
        std::cout << "========================================\n";
    }

    // ==========================================
    // PRUEBA: MAYOR BETWEENNESS CENTRALITY
    // ==========================================
    auto resultados_bc = AnalizadorCentralidad<std::string, AristaData>::calcularBetweennessCentrality(
        redIoT,
        AristaData::getPesoMinimo
    );

    if (!resultados_bc.empty()) {
        int id_max_bc = -1;
        double max_score_bc = -1.0; 

        for (const auto& par : resultados_bc) {
            if (par.second > max_score_bc) {
                max_score_bc = par.second;
                id_max_bc = par.first;
            }
        }

        std::string nombre_ganador_bc = redIoT.getVertexElement(id_max_bc);

        std::cout << "\n========================================\n";
        std::cout << " MAYOR BETWEENNESS CENTRALITY (Nodo Puente)\n";
        std::cout << "========================================\n";
        std::cout << "Red evaluada: IoT\n";
        std::cout << "Nodo (IP): " << nombre_ganador_bc << "\n";
        std::cout << "Score: " << std::fixed << std::setprecision(4) << max_score_bc << "\n";
        std::cout << "========================================\n";
    }

    // ==========================================
    // PRUEBA: MAYOR CLOSENESS CENTRALITY
    // ==========================================
    auto resultados_cc = AnalizadorCentralidad<std::string, AristaData>::ClosenessCentrality(
        redNetScience,
        -1,
        true,
        AristaData::getPesoMinimo
    );

    if (!resultados_cc.empty()) {
        int id_max_cc = -1;
        double max_score_cc = -1.0; 

        for (const auto& par : resultados_cc) {
            if (par.second > max_score_cc) {
                max_score_cc = par.second;
                id_max_cc = par.first;
            }
        }

        std::string nombre_ganador_cc = redNetScience.getVertexElement(id_max_cc);

        std::cout << "\n========================================\n";
        std::cout << " NODO CON MAYOR CLOSENESS CENTRALITY\n";
        std::cout << "========================================\n";
        std::cout << "Red evaluada: NetScience\n";
        std::cout << "Nodo: " << nombre_ganador_cc << "\n";
        std::cout << "Score: " << std::fixed << std::setprecision(4) << max_score_cc << "\n";
        std::cout << "========================================\n";
    } else {
        std::cout << "La red de NetScience esta vacia.\n";
    }

    // ==========================================
    // PRUEBA: AVERAGE SHORTEST PATH
    // ==========================================

    // Prueba Ponderada (Ej. NetScience)
    double avg_path_netscience = AnalizadorCentralidad<std::string, AristaData>::AverageShortestPath(
        redNetScience,
        AristaData::getPesoMinimo
    );
    
    std::cout << "\n\n========================================\n";
        std::cout << " Average Shortest Path\n";
        std::cout << "========================================\n";
        std::cout << "Red evaluada: NetScience\n";
        std::cout << "Score: " << std::fixed << std::setprecision(4) << avg_path_netscience << "\n";
        std::cout << "========================================\n";
    */
    /* ESTA PRUEBA DEMORA COMO 30min xd
    // Prueba NO Ponderada (Ej. Actores - No requiere función de peso)
    double avg_path_actores = AnalizadorCentralidad<std::string, AristaData>::AverageShortestPath(
        redActores,
        nullptr
    );
    */
   
    // ==========================================
    // MEDIDAS Y RENDIMIENTO
    // ==========================================
    // PageRank - Red Actores
    auto resPR = AnalizadorCentralidad<std::string, AristaData>::calcularPageRank(redActores);
    medirRendimientoCentralidad(redActores, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularPageRank(G);
    }, "PageRank - Red de Actores");

    // Degree Centrality - Red IoT
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularDegreeCentrality(G);
    }, "Degree Centrality - Red IoT");

    // Betweenness Centrality - Red IoT (requiere extractor de peso)
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularBetweennessCentrality(G, AristaData::getPesoTotal);
    }, "Betweenness Centrality - Red IoT");

    // Closeness Centrality - Red NetScience
    medirRendimientoCentralidad(redNetScience, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularClosenessCentrality(G);
    }, "Closeness Centrality - Red NetScience");

    // Average Shortest Path - Red NetScience
    medirRendimientoCentralidad(redNetScience, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularAverageShortestPath(G);
    }, "Average Shortest Path - Red NetScience");

    // Eigenvector Centrality - Red IoT
    medirRendimientoCentralidad(redIoT, [](auto& G) {
        auto res = AnalizadorCentralidad<std::string, AristaData>::calcularEigenvectorCentrality(G);
    }, "Eigenvector Centrality - Red IoT");

    // Clustering Local (Cálculo para todos los nodos) - Red NetScience
    medirRendimientoCentralidad(redNetScience, [](auto& G) {
        for (int v : G.vertices()) {
            AnalizadorCentralidad<std::string, AristaData>::calcularClustering(G, v);
        }
    }, "Clustering Local (Todos los nodos) - Red NetScience");

    return 0;
}