#pragma once

#include <string>
#include "grafoADT.h"
#include "extern/csv.h"

namespace DatasetLoader {

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

                grafo.insertEdge(id_v1, id_v2, peso_arista);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "error: " << nombreArchivo << "no ha cargado correctamente: " << e.what() << std::endl;
        }
    }
}