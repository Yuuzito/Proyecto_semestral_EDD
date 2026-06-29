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


    /**
     * @brief Calcula el número de triángulos que pasan por un vértice u en un grafo no dirigido no ponderado.
     *
     * @param G Referencia al grafo.
     * @param u ID del vértice a evaluar.
     * @param vecinos Vector con los IDs de los vecinos directos de u.
     *
     * @return double Número de triángulos que incluyen al vértice u.
     */
    static double contarTriangulos(Grafo<VType, EType>& G, int u, const std::vector<int>& vecinos) {
        int triangulos = 0.0;

        // Verificar cada par único de vecinos
        for (size_t i = 0; i < vecinos.size(); ++i) {
            for (size_t j = i + 1; j < vecinos.size(); ++j) {

                // Si el vecino "i" y el vecino "j" están conectados entre sí, entonces forman un triángulo junto con "u".
                if (G.areAdjacent(vecinos[i], vecinos[j])) {
                    triangulos++;
                }
            }
        }

        return triangulos;
    }


    /**
     * @brief Calcula el número de triángulos que pasan por un vértice u en un grafo dirigido no ponderado.
     *
     * @param G Referencia al grafo.
     * @param u ID del vértice a evaluar.
     * @param vecinos Vector con los IDs de los vecinos únicos (entrada y salida).
     *
     * @return double Número de triángulos dirigidos que incluyen al vértice u.
     */
    static double contarTriangulosDirigidos(Grafo<VType, EType>& G, int u, const std::vector<int>& vecinos) {
        double triangulos = 0.0;

        // Verificar cada par único de vecinos
        for (size_t i = 0; i < vecinos.size(); ++i) {
            for (size_t j = i + 1; j < vecinos.size(); ++j) {
                int v = vecinos[i];
                int w = vecinos[j];

                // d_xy = número de conexiones entre x e y (0, 1 o 2)
                double d_uv = (G.areAdjacent(u, v) ? 1.0 : 0.0) + (G.areAdjacent(v, u) ? 1.0 : 0.0);
                double d_uw = (G.areAdjacent(u, w) ? 1.0 : 0.0) + (G.areAdjacent(w, u) ? 1.0 : 0.0);
                double d_vw = (G.areAdjacent(v, w) ? 1.0 : 0.0) + (G.areAdjacent(w, v) ? 1.0 : 0.0);

                
                // Dado que el grado de conexión es bidireccional (d_vw == d_wv), la multiplicación resultante es idéntica en ambas direcciones.
                // Multiplicamos por 2.0 por las permutaciones (v, w) y (w, v)
                triangulos += 2.0 * (d_uv * d_uw * d_vw);
            }
        }

        return triangulos;
    }


    /**
     * @brief Encuentra el peso máximo entre todas las aristas de un grafo ponderado.
     *
     * @param G Referencia al grafo.
     * @param funcionPeso Puntero a función para extraer el peso de la arista.
     *
     * @return double El peso máximo encontrado. Si no hay aristas, retorna 0.0.
     */
    static double getPesoMaximo(Grafo<VType, EType>& G, double (*funcionPeso)(EType)) {
        double max_w = 0.0;
        for (int e : G.edges()) {
            double peso = funcionPeso(G.getEdgeElement(e));
            if (peso > max_w) {
                max_w = peso;
            }
        }
        return max_w;
    }
};