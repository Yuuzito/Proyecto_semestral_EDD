#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "grafoADT.h"

template <typename V, typename E, typename ParserFunc>
void cargarDatasetGeneral(Grafo<V, E>& grafo, const std::string& ruta, bool saltar_header, ParserFunc parser) {
    std::ifstream archivo(ruta); // Abrimos el archivo
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir " << ruta << "\n";
        return;
    }

    std::string linea;

    // Si el dataset tiene una fila de encabezado, la saltamos.
    if (saltar_header) {
        std::getline(archivo, linea);
    }

    std::unordered_map<V, int> historial_vertices; // Para mapear el valor del vértice a su ID interno en el grafo.
    int lineas_validas = 0;

    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string celda;
        std::vector<std::string> columnas;
        
        while (std::getline(ss, celda, ',')) {
            columnas.push_back(celda);
        }

        V src_val, dst_val; // Vertices de origen y destino
        E peso_val; // Peso de la arista

        // Llamamos a la función para que extraiga los datos. Si retorna true, la línea era válida.
        if (parser(columnas, src_val, dst_val, peso_val)) {
            
            // Verificamos e insertamos Vértice Origen
            if (historial_vertices.find(src_val) == historial_vertices.end()) {
                historial_vertices[src_val] = grafo.insertVertex(src_val);
            }
            // Verificamos e insertamos Vértice Destino
            if (historial_vertices.find(dst_val) == historial_vertices.end()) {
                historial_vertices[dst_val] = grafo.insertVertex(dst_val);
            }

            // Insertamos la arista usando los IDs internos
            grafo.insertEdge(historial_vertices[src_val], historial_vertices[dst_val], peso_val);
            lineas_validas++;
        }
    }

    archivo.close();
    std::cout << "Archivo " << ruta << " cargado. Aristas leidas: " << lineas_validas 
              << " | Vertices unicos: " << grafo.numVertices() << "\n";
}