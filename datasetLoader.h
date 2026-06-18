#pragma once

#include <string>
#include "grafoADT.h"
#include "extern/csv.h"

namespace DatasetLoader {

    // Función auxiliar actualizada para manejar la dualidad de métricas
    template <typename V, typename E>
    void insertarOSumarArista(Grafo<V, E>& grafo, int id_v1, int id_v2, E peso_nuevo) {
        int id_arista = grafo.getEdge(id_v1, id_v2);
        
        if (id_arista != -1) {
            // 1. Lógica para Degree / PageRank: Acumular el peso total
            E peso_actual = grafo.getEdgeElement(id_arista);
            grafo.replaceEdge(id_arista, peso_actual + peso_nuevo);
            
            // 2. Lógica para Closeness / Betweenness: Mantener el valor más bajo
            E min_actual = grafo.getEdgeMinimum(id_arista);
            if (peso_nuevo < min_actual) {
                grafo.replaceEdgeMinimum(id_arista, peso_nuevo);
            }
        } else {
            // Si la arista no existe, insertEdge se encarga de inicializar 
            // tanto la suma como el mínimo con 'peso_nuevo'
            grafo.insertEdge(id_v1, id_v2, peso_nuevo);
        }
    }

    template <typename V, typename E>
    void cargarDatasetCSV(
        const std::string& nombreArchivo,
        Grafo<V, E>& grafo,
        const char* columnaOrigen,
        const char* columnaDestino,
        const char* columnaPeso
    ) {
        try {
            io::CSVReader<3> in(nombreArchivo);
            in.read_header(io::ignore_extra_column, columnaOrigen, columnaDestino, columnaPeso);

            std::string origen;
            std::string destino;
            std::string peso_str;

            while (in.read_row(origen, destino, peso_str)) {
                int id_v1 = grafo.insertVertex(origen);
                int id_v2 = grafo.insertVertex(destino);
                
                // Conversión segura para soportar tanto números normales como guiones "-"
                double peso = 0.0;
                if (peso_str != "-" && !peso_str.empty()) {
                    try {
                        peso = std::stod(peso_str);
                    }
                    catch (...) {
                        peso = 0.0;
                    }
                }
                
                E peso_arista = static_cast<E>(peso);

                insertarOSumarArista(grafo, id_v1, id_v2, peso_arista);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "error: " << nombreArchivo << "no ha cargado correctamente: " << e.what() << std::endl;
        }
    }
}