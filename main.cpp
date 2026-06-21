#include <iostream>
#include <iomanip> // Para std::setprecision
#include <string>

#include "grafoADT.h"
#include "grafoRed.h"
#include "datasetLoader.h"
#include "medidasCentralidad.h"
#include "rendimientoCentralidad.h"

int main() {
    // ==========================================
    // 1. INSTANCIACIÓN DE LAS REDES
    // ==========================================
    GrafoRed<std::string> redActores(false);    // false = no dirigido
    GrafoRed<std::string> redIoT(true);         // true = dirigido
    GrafoRed<std::string> redNetScience(false); // false = no dirigido (Toy Dataset)

    // ==========================================
    // 2. CARGAR DATASETS
    // ==========================================
    std::cout << "Cargando datasets...\n";

    // Selección de rutas para Windows y Linux
    #ifdef _WIN32
        std::string ruta_Actores = "..\\datasets\\imdb_edgelist.csv";
        std::string ruta_IoT = "..\\datasets\\train_test_network.csv";
        std::string ruta_NetScience = "..\\datasets\\NetScience.csv";
    #else
        // Sin el "../" porque tu ejecutable y la carpeta datasets están al mismo nivel
        std::string ruta_Actores = "datasets/imdb_edgelist.csv";
        std::string ruta_IoT = "datasets/train_test_network.csv";
        std::string ruta_NetScience = "datasets/NetScience.csv";
    #endif

    DatasetLoader::cargarDatasetCSV(ruta_Actores, redActores, "From", "To", "Strength");
    DatasetLoader::cargarDatasetCSV(ruta_IoT, redIoT, "src_ip", "dst_ip", "duration");
    DatasetLoader::cargarDatasetCSV(ruta_NetScience, redNetScience, "Source", "Target", "Weight");

    std::cout << "Datasets cargados exitosamente.\n";
    std::cout << "Red de Actores: " << redActores.getGrafo().numVertices() << " nodos, " << redActores.getGrafo().edges().size() << " conexiones.\n";
    std::cout << "Red de IoT: " << redIoT.getGrafo().numVertices() << " nodos, " << redIoT.getGrafo().edges().size() << " conexiones.\n";
    std::cout << "Red NetScience: " << redNetScience.getGrafo().numVertices() << " nodos, " << redNetScience.getGrafo().edges().size() << " conexiones.\n";

    // ==========================================
    // 3. BUSCAR EL NODO CON MAYOR PAGERANK
    // ==========================================
    auto resultados_pr = AnalizadorCentralidad<std::string>::calcularPageRank(redActores);

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
        auto& grafo = redActores.getGrafo();
        std::string nombre_ganador = grafo.getVertexElement(id_max);

        std::cout << "\n========================================\n";
        std::cout << "  EL NODO MAS IMPORTANTE (MAYOR PAGERANK)\n";
        std::cout << "========================================\n";
        std::cout << "Nodo: " << nombre_ganador << "\n";
        std::cout << "Score: " << std::fixed << std::setprecision(4) << max_score << "\n";
        std::cout << "========================================\n";
    } else {
        std::cout << "La red de Actores esta vacia, no hay PageRank que mostrar.\n";
    }

    // Prueba de Funcionalidad: Encontrar el nodo más "cercano" al resto de la red
    auto resultados_cc = AnalizadorCentralidad<std::string>::ClosenessCentrality(redNetScience);

    if (!resultados_cc.empty()) {
        int id_max_cc = -1;
        double max_score_cc = -1.0; 

        for (const auto& par : resultados_cc) {
            if (par.second > max_score_cc) {
                max_score_cc = par.second;
                id_max_cc = par.first;
            }
        }

        auto& grafo_ns = redNetScience.getGrafo();
        std::string nombre_ganador_cc = grafo_ns.getVertexElement(id_max_cc);

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
    // 4. PRUEBAS DE RENDIMIENTO DE CENTRALIDAD
    // ==========================================
    std::cout << "\n=======================================================\n";
    std::cout << "   INICIANDO PRUEBAS DE TIEMPO\n";
    std::cout << "=======================================================\n";

    // Medición de PageRank (Red de Actores) - 10 iteraciones
    medirRendimientoCentralidad(redActores, [](GrafoRed<std::string>& red) {
        auto pr = AnalizadorCentralidad<std::string>::calcularPageRank(red);
    }, "PageRank - Red de Actores", 10);

    // Medición de Degree Centrality (Red de Actores) - 10 iteraciones
    medirRendimientoCentralidad(redActores, [](GrafoRed<std::string>& red) {
        auto dc = AnalizadorCentralidad<std::string>::calcularDegreeCentrality(red);
    }, "Degree Centrality - Red de Actores", 10);

    // Medición de Betweenness Centrality (Red NetScience) - 1 iteracion
    // Usamos el Toy Dataset para no desbordar los tiempos de cómputo
    medirRendimientoCentralidad(redNetScience, [](GrafoRed<std::string>& red) {
        auto bc = AnalizadorCentralidad<std::string>::calcularBetweennessCentrality(red);
        }, "Betweenness Centrality - Red NetScience (Toy Dataset)", 10);

    // Prueba de Rendimiento (10 iteraciones en el Toy Dataset)
    medirRendimientoCentralidad(redNetScience, [](GrafoRed<std::string>& red) {
        auto cc = AnalizadorCentralidad<std::string>::ClosenessCentrality(red);
    }, "Closeness Centrality - Red NetScience", 10);

    return 0;
}