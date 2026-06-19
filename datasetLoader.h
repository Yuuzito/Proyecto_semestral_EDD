#pragma once

#include <string>
#include "grafoADT.h"
#include "grafoRed.h"
#include "extern/csv.h"

namespace DatasetLoader {

    /**
     * @brief Carga los datos de un archivo CSV directamente en una instancia de GrafoRed.
     *
     * @tparam V Tipo de dato que representan los vértices (ej. string).
     * @param nombreArchivo Ruta o nombre del archivo CSV a procesar.
     * @param red Referencia al GrafoRed que almacenará los datos.
     * @param columnaOrigen Nombre del header en el CSV para el nodo de origen.
     * @param columnaDestino Nombre del header en el CSV para el nodo de destino.
     * @param columnaPeso Nombre del header en el CSV para el peso de la arista.
     */
    template <typename V>
    void cargarDatasetCSV(
        const std::string& nombreArchivo,
        GrafoRed<V>& red,
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
                int id_v1 = red.insertarNodo(origen);
                int id_v2 = red.insertarNodo(destino);

                // Conversión segura para el peso (Parseo del peso)
                double peso = 0.0;
                if (peso_str != "-" && !peso_str.empty()) {
                    try {
                        peso = std::stod(peso_str);
                    }
                    catch (...) {
                        peso = 0.0;
                    }
                }
                
                red.insertarConexion(id_v1, id_v2, peso);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "error: " << nombreArchivo << "no ha cargado correctamente: " << e.what() << std::endl;
        }
    }
}