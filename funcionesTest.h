#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "grafoADT.h"

// Función para buscar el ID interno de un vértice a partir de su contenido (ej. la IP)
template <typename V, typename E>
int buscarIdPorValor(Grafo<V, E>& grafo, const V& valor_buscado) {
    for (int id : grafo.vertices()) {
        if (grafo.getVertexElement(id) == valor_buscado) {
            return id;
        }
    }
    return -1; // Retorna -1 si no lo encuentra
}

// Función para imprimir como "Rayos X" lo que hay dentro de un vértice
template <typename V, typename E>   
void inspeccionarNodo(Grafo<V, E>& grafo, const V& valor, long long unsigned int max_conexiones_a_mostrar) {
    int id_nodo = buscarIdPorValor(grafo, valor);
    
    if (id_nodo == -1) {
        std::cout << "El nodo '" << valor << "' NO existe en el grafo.\n";
        return;
    }

    std::vector<int> aristas = grafo.incidentEdges(id_nodo);
    
    std::cout << "\nNODO ENCONTRADO: '" << valor << "' (ID interno: " << id_nodo << ")\n";
    std::cout << "Tiene " << aristas.size() << " conexiones directas:\n";
    
    // Imprimir hasta un máximo de conexiones que desee el usuario
    long long unsigned int limite = std::min((long long unsigned int)aristas.size(), max_conexiones_a_mostrar);
    
    for (long long unsigned int i = 0; i < limite; i++) {
        int id_arista = aristas[i];
        std::pair<int, int> extremos = grafo.endVertices(id_arista);
        
        // Determinar quién es el vecino 
        int id_vecino = (extremos.first == id_nodo) ? extremos.second : extremos.first;
        
        std::cout << "  -> Conectado con: " << grafo.getVertexElement(id_vecino) 
                  << " | Peso Total: " << grafo.getEdgeElement(id_arista).peso_total 
                  << " | Peso Minimo: " << grafo.getEdgeElement(id_arista).peso_minimo << "\n";
    }
    
    if (aristas.size() > max_conexiones_a_mostrar) {
        std::cout << "  ... y " << (aristas.size() - max_conexiones_a_mostrar) << " conexiones mas.\n";
    }
}