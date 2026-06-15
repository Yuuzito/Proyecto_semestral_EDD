#include <iostream>
#include "grafoADT.h"
#include "lecturaDataSet.cpp"

int main() {
    Grafo<std::string, int> red_actores(false); // false = no dirigido.
    
    // Funcion de lectura para el dataset de IMDb. Extrae Actor1, Actor2 y Cantidad de Colaboraciones (peso).
    auto parser_imdb = [](const std::vector<std::string>& cols, std::string& src, std::string& dst, int& peso) {
        if (cols.size() < 3) return false; // Ignorar líneas rotas.
        
        src = cols[0]; // Actor 1
        dst = cols[1]; // Actor 2
        try {
            peso = std::stoi(cols[2]); // Convertir texto a Int.
            return true;
        } catch (...) { return false; }
    };

    std::cout << "Cargando IMDb...\n";
    cargarDatasetGeneral(red_actores, "..\\datasets\\imdb_edgelist.csv", true, parser_imdb);

    Grafo<std::string, double> red_iot(true); // true = dirigido.

    // Funcion de lectura para el dataset de IoT. Extrae src_ip, dst_ip y duration (peso).
    auto parser_train_test = [](const std::vector<std::string>& columnas, std::string& origen, std::string& destino, double& peso) {
        
        // Verificamos que la fila tenga al menos 7 elementos.
        if (columnas.size() < 7) {
            return false; 
        }
        
        origen  = columnas[0];  // Extraemos src_ip.
        destino = columnas[2];  // Extraemos dst_ip.
        try {
            peso = std::stod(columnas[6]); // Extraemos duration y lo pasamos a decimal.
        } catch (...) {
            // Si el texto de duration viene con un guión "-" o un error, le ponemos 0.
            peso = 0.0; 
        }

        return true; // Si es valida creamos la arista.
    };

    std::cout << "\nCargando IoT...\n";
    cargarDatasetGeneral(red_iot, "..\\datasets\\train_test_network.csv", true, parser_train_test);

    // Inspeccionamos algunos nodos para verificar que todo esté bien construido
    inspeccionarNodo(red_iot, std::string("2405:6e00:10ce:2c00:9064:17:85d2:57d4"), 20);

    return 0;
}