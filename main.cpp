#include <iostream>
#include <chrono>
#include "grafoADT.h"
#include "lecturaDataSet.cpp"
#include "datasetLoader.h"

#include <chrono>
#include "extern/csv.h"

int main() {
    // Instanciamos grafos
    Grafo<std::string, int> redActores(false);  // false = no dirigido
    Grafo<std::string, double> redIoT(true);

    // Cargar dataset
    std::cout << "Cargando datasets...\n";
    DatasetLoader::cargarDatasetCSV("datasets/imdb_edgelist.csv", redActores, "From", "To", "Strength");
    DatasetLoader::cargarDatasetCSV("datasets/train_test_network.csv", redIoT, "src_ip", "dst_ip", "duration");

    // Inspección de algunos nodos para verificar correctitud en la construcción de la red
    inspeccionarNodo(redActores, std::string("nm1568415"), 20);
    inspeccionarNodo(redIoT, std::string("192.168.1.37"), 20);

    return 0;
}