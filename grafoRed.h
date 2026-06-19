#pragma once

#include "grafoADT.h"

template<typename VType>
class GrafoRed {
private:
    /**
     * @brief Estructura para los datos que guarda cada arista de la red.
     */
    struct DatosArista {
        double peso_total;      // Suma de todos los pesos registrados (por ej. uso para Degree Centrality)
        double peso_minimo;     // Peso minimo registrado   (por ej. uso para caminos más cortos)
    };

    // Instancia interna del grafo de red
    Grafo<VType, DatosArista> grafoInterno;

public:
    // Constructor
    GrafoRed(bool dirigido) : grafoInterno(dirigido) {}

    // Getter para hacer uso externo de los métodos del grafoADT
    Grafo<VType, DatosArista>& getGrafo() {
        return grafoInterno;
    }

    // Inserta un nodo y devuelve su ID interno
    int insertarNodo(VType valor) {
        return grafoInterno.insertVertex(valor);
    }

    // Inserta una conexión en la red
    void insertarConexion(int id_v1, int id_v2, double peso) {
        int id_arista = grafoInterno.getEdgeID(id_v1, id_v2);
        
        if (id_arista != -1) {
            // Si ya existe, actualizamos los datos de la Arista
            DatosArista datos = grafoInterno.getEdgeElement(id_arista);
            datos.peso_total += peso;
            if (peso < datos.peso_minimo) {
                datos.peso_minimo = peso;
            }
            grafoInterno.replaceEdge(id_arista, datos);
        }
        else {
            // Si es nueva, inicializamos ambos valores
            grafoInterno.insertEdge(id_v1, id_v2, { peso, peso });
        }
    }

    // ==========================================
    // MEDIDAS DE CENTRALIDAD
    // ==========================================

};