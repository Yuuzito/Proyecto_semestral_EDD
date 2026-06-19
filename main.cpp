#include <iostream>

#include "grafoADT.h"
#include "grafoRed.h"
#include "datasetLoader.h"

int main() {
    // Instanciamos redes
    GrafoRed<std::string> redActores(false);    // false = no dirigido
    GrafoRed<std::string> redIoT(true);

    // Cargar datasets
    std::cout << "Cargando datasets...\n";
    DatasetLoader::cargarDatasetCSV("datasets/imdb_edgelist.csv", redActores, "From", "To", "Strength");
    DatasetLoader::cargarDatasetCSV("datasets/train_test_network.csv", redIoT, "src_ip", "dst_ip", "duration");

    // Inspección de algunos nodos para verificar correctitud en la construcción de la red
    inspeccionarNodo(redActores.getGrafo(), std::string("nm1568415"), 20);
    inspeccionarNodo(redIoT.getGrafo(), std::string("192.168.1.193"), 20);
    inspeccionarNodo(redIoT.getGrafo(), std::string("192.168.1.37"), 20);

    return 0;
}