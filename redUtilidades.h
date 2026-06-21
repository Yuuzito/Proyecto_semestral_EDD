#pragma once

#include <string>
#include "grafoADT.h"
#include "extern/csv.h"

struct AristaData {
    double peso_total;
    double peso_minimo;

    // =======
    // Métodos estáticos usados como funciones extractoras
    // =======

    static double getPesoMinimo(AristaData arista) {
        return arista.peso_minimo;
    }

    static double getPesoTotal(AristaData arista) {
        return arista.peso_total;
    }
};

class RedUtilidades {
private:
    // ==========================================
    // MÉTODOS AUXILIARES
    // ==========================================

    static AristaData crearArista(double peso) {
        AristaData nuevaArista;
        nuevaArista.peso_total = peso;
        nuevaArista.peso_minimo = peso;
        return nuevaArista;
    }

    static void actualizarAristaExistente(AristaData& actual, double nuevoPeso) {
        actual.peso_total += nuevoPeso;
        if (nuevoPeso < actual.peso_minimo) actual.peso_minimo = nuevoPeso;
    }

public:
    /**
     * @brief Carga los datos de un archivo CSV directamente en una instancia de GrafoRed.
     *
     * @tparam VType Tipo de dato que representan los vértices (ej. string).
     *
     * @param nombreArchivo Ruta o nombre del archivo CSV a procesar.
     * @param red Referencia al GrafoRed que almacenará los datos.
     * @param columnaOrigen Nombre del header en el CSV para el nodo de origen.
     * @param columnaDestino Nombre del header en el CSV para el nodo de destino.
     * @param columnaPeso Nombre del header en el CSV para el peso de la arista.
     */
    template <typename VType>
    static void cargarDatasetCSV(
        const std::string& nombreArchivo,
        Grafo<VType, AristaData>& G,
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
                int id_v1 = G.insertVertex(origen);
                int id_v2 = G.insertVertex(destino);

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

                int id_arista = G.getEdgeID(id_v1, id_v2);

                if (id_arista != -1) {
                    // CASO 1: Si la arista ya existe, actualizamos su struct y reemplazamos

                    AristaData dataActual = G.getEdgeElement(id_arista);
                    actualizarAristaExistente(dataActual, peso);
                    G.replaceEdge(id_arista, dataActual);
                }
                else {
                    // CASO 2: Si la arista no existe, creamos su struct e insertamos
                    
                    AristaData newData = crearArista(peso);
                    G.insertEdge(id_v1, id_v2, newData);
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "error: " << nombreArchivo << " no ha cargado correctamente: " << e.what() << std::endl;
        }
    }
};