#pragma once

#include "grafoADT.h"
#include <queue>
#include <unordered_map>
#include <limits>

const double INF = std::numeric_limits<double>::infinity();

template <typename VType, typename EType>
class GrafoUtilidades {

public:
    /**
     * @brief Calcula las distancias más cortas desde un vértice de origen hacia todos los demás en un grafo no ponderado.
     *
     * @param G Referencia al grafo a explorar.
     * @param s ID del vértice de origen desde donde inicia la búsqueda.
     * @return std::unordered_map<int, int> Mapa con el par clave-valor: ID del vértice y su distancia mínima.
     */
    static std::unordered_map<int, double> BFS(Grafo<VType, EType>& G, int s) {
        std::unordered_map<int, bool> visitado;
        std::unordered_map<int, double> distancia;
        std::unordered_map<int, int> padre;     // Guarda el camino más corto

        std::queue<int> Q;

        for (int u : G.vertices()) {
            // Inicializamos variables
            visitado[u] = false;
            distancia[u] = INF;
            padre[u] = -1;  // Usamos -1 para representar Null (aprovechamos que los IDs válidos son >= 0)
        }

        // Inicializamos el vértice de origen "s"
        visitado[s] = true;
        distancia[s] = 0.0;
        Q.push(s);

        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();

            // Buscamos todos los vecinos de u
            for (int e : G.incidentEdges(u)) {
                int v = G.opposite(u, e);

                // Si "v" no ha sido visitado
                if (!visitado[v]) {
                    // Actualizar vecino y agregar a la cola
                    visitado[v] = true;
                    distancia[v] = distancia[u] + 1;
                    padre[v] = u;
                    Q.push(v);
                }
            }
        }

        return distancia;
    }

    /**
     * @brief Calcula las distancias mínimas desde un vértice de origen hacia todos los demás evaluando el peso de las aristas.
     *
     * @tparam AtributoPeso Puntero al miembro del EType que se usa como peso.
     *
     * @param G Referencia al grafo ponderado a explorar.
     * @param s ID del vértice de origen desde donde inicia la búsqueda.
     * @param funcionPeso Puntero a función normal para extraer el peso de la arista.
     * @return std::unordered_map<int, double> Mapa con el par clave-valor: ID del vértice y su distancia mínima.
     */
    static std::unordered_map<int, double> DijkstraShortestPath(
        Grafo<VType, EType>& G,
        int s,
        double (*funcionPeso)(EType)
    ) {
        std::unordered_map<int, bool> visitado;
        std::unordered_map<int, double> distancia;
        std::unordered_map<int, int> padre;

        // Alias para el par {distancia, id_vertice}
        using Par = std::pair<double, int>;

        // Cola de prioridad
        std::priority_queue <Par, std::vector<Par>, std::greater<Par>> Q;

        for (int u : G.vertices()) {
            visitado[u] = false;
            distancia[u] = INF;
            padre[u] = -1;
        }

        // Inicializamos el vértice de origen
        distancia[s] = 0.0;
        Q.push({ 0.0, s });

        while (!Q.empty()) {
            // Q.removeMin(): extraemos el vértice con la menor distancia.
            Par u_actual = Q.top();
            Q.pop();

            double dist_u = u_actual.first;
            int u = u_actual.second;

            // Lazy Deletion: Si sacamos un par obsoleto (fantasma), no hacemos nada
            if (dist_u > distancia[u]) {
                continue;
            }

            visitado[u] = true;

            // Relajación de aristas: Buscamos todos los vecinos de u
            for (int e : G.incidentEdges(u)) {
                int v = G.opposite(u, e);

                // Acceso al atributo de peso especificado
                double peso = funcionPeso(G.getEdgeElement(e));

                if (distancia[u] + peso < distancia[v]) {
                    distancia[v] = distancia[u] + peso;
                    padre[v] = u;
                    
                    // Insertamos la nueva distancia más corta en la cola
                    Q.push({distancia[v], v}); 
                }
            }
        }

        return distancia;
    }



};