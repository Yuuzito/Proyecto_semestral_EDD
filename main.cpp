#include <iostream>
#include <iomanip>
#include <string>

#include "grafoADT.h"
#include "redUtilidades.h"
#include "medidasCentralidad.h"
#include "rendimientoCentralidad.h"


int main() {
    // ==========================================
    // 1. INSTANCIACIÓN DE LAS REDES
    // ==========================================
    Grafo<std::string, AristaData> redActores(false);       // false = no dirigido
    Grafo<std::string, AristaData> redIoT(true);            // true = dirigido
    Grafo<std::string, AristaData> redNetScience(false);    // false = no dirigido

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

    RedUtilidades::cargarDatasetCSV(ruta_Actores, redActores, "From", "To", "Strength");
    RedUtilidades::cargarDatasetCSV(ruta_IoT, redIoT, "src_ip", "dst_ip", "duration");
    RedUtilidades::cargarDatasetCSV(ruta_NetScience, redNetScience, "Source", "Target", "Weight");

    std::cout << "Datasets cargados exitosamente.\n";
    std::cout << "Red de Actores: " << redActores.numVertices() << " nodos, " << redActores.edges().size() << " conexiones.\n";
    std::cout << "Red de IoT: " << redIoT.numVertices() << " nodos, " << redIoT.edges().size() << " conexiones.\n";
    std::cout << "Red NetScience: " << redNetScience.numVertices() << " nodos, " << redNetScience.edges().size() << " conexiones.\n";

    return 0;
}