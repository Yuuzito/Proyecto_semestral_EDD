#include <iostream>

#include "grafoADT.h"
#include "grafoRed.h"
#include "datasetLoader.h"
#include "rendimientoCentralidad.h"

int main() {
    // Instanciamos redes
    GrafoRed<std::string> redActores(false);    // false = no dirigido
    GrafoRed<std::string> redIoT(true);

    // Cargar datasets
    std::cout << "Cargando datasets...\n";
    DatasetLoader::cargarDatasetCSV("..\\datasets\\imdb_edgelist.csv", redActores, "From", "To", "Strength");
    DatasetLoader::cargarDatasetCSV("..\\datasets\\train_test_network.csv", redIoT, "src_ip", "dst_ip", "duration");

    std::cout << "Datasets cargados exitosamente.\n";
    std::cout << "Red de Actores: " << redActores.getGrafo().numVertices() << " nodos, " << redActores.getGrafo().edges().size() << " conexiones.\n";
    std::cout << "Red de IoT: " << redIoT.getGrafo().numVertices() << " nodos, " << redIoT.getGrafo().edges().size() << " conexiones.\n";

    // ... [Asumiendo que ya corriste tu algoritmo así:]
    auto resultados_pr = AnalizadorCentralidad<std::string>::calcularPageRank(redActores);

    // ==========================================
    // BUSCAR EL NODO CON MAYOR PAGERANK
    // ==========================================
    if (!resultados_pr.empty()) {
        int id_max = -1;
        double max_score = -1.0; // Empezamos con un valor muy bajo

        // Recorremos todos los resultados
        for (const auto& par : resultados_pr) {
            int id_actual = par.first;
            double score_actual = par.second;

            // Si encontramos un score mayor, actualizamos a nuestro "ganador"
            if (score_actual > max_score) {
                max_score = score_actual;
                id_max = id_actual;
            }
        }

        // Recuperamos el nombre del nodo usando su ID
        auto& grafo = redActores.getGrafo();
        std::string nombre_ganador = grafo.getVertexElement(id_max);

        // Imprimimos el resultado destacado
        std::cout << "\n========================================\n";
        std::cout << "  EL NODO MAS IMPORTANTE (MAYOR PAGERANK)\n";
        std::cout << "========================================\n";
        std::cout << "Nodo: " << nombre_ganador << "\n";
        std::cout << "Score: " << std::fixed << std::setprecision(4) << max_score << "\n";
        std::cout << "========================================\n";
    } else {
        std::cout << "La red esta vacia, no hay PageRank que mostrar.\n";
    }
    medirRendimientoCentralidad(redActores, [](GrafoRed<std::string>& red) {
        auto pr = AnalizadorCentralidad<std::string>::calcularPageRank(red);
    }, "PageRank - Red de Actores");

    return 0;
}