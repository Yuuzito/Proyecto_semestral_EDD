#include <iostream>
#include <iomanip>
#include <string>

#include "grafoADT.h"
#include "redUtilidades.h"
#include "medidasCentralidad.h"
#include "rendimientoCentralidad.h"
#include "experimentos.h"


int main() {
    // ==========================================
    // INSTANCIACIÓN DE LAS REDES
    // ==========================================
    Grafo<std::string, AristaData> redNetScience(false);    // false = no dirigido

    // ==========================================
    // CARGAR DATASETS
    // ==========================================
    std::cout << "\nCargando datasets...\n";

    // Selección de rutas para Windows y Linux
    #ifdef _WIN32
        std::string ruta_NetScience = "..\\datasets\\NetScience.csv";
    #else
        std::string ruta_NetScience = "datasets/NetScience.csv";
    #endif

    RedUtilidades::cargarDatasetCSV(ruta_NetScience, redNetScience, "Source", "Target", "Weight");

    std::cout << "========================================\n";
    std::cout << "Datasets cargados exitosamente.\n";
    std::cout << "========================================\n";

    AristaData aristaBase;
    aristaBase.peso_total = 1.0;
    aristaBase.peso_minimo = 1.0;

    // Extraer funcion de peso
    auto funcionPeso = [](AristaData a) { return a.peso_total; };

    // Correr todos los experimentos para un dataset
    ExperimentosSensibilidad<std::string, AristaData>::ejecutarTodos(redNetScience, funcionPeso, aristaBase);
    return 0;
}