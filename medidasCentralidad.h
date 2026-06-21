#pragma once

#include "grafoADT.h"
#include "grafoUtilidades.h"
#include <unordered_map>
#include <vector>
#include <cmath> // Para std::abs
#include <queue>   
#include <stack>   
#include <limits>  

template<typename VType, typename EType>
class AnalizadorCentralidad {
private:
    /**
     * @brief Método auxiliar para calcular Closeness Centrality (Centralida de cercanía)
     *
     */
    static double calcularClosenessCentrality(
        Grafo<VType, EType>& G,
        int u,
        int num_vertices,
        bool esPonderado,
        bool esConexo,
        double (*funcionPeso)(EType)    
    ) {
        double sum_distancias = 0.0;
        int nodos_alcanzables = 0;

        if (!esPonderado) {
            // Usar BFS
            auto distancias = GrafoUtilidades<VType, EType>::BFS(G, u);
            for (const auto& par : distancias) {
                if (par.second != INF && par.second != INT_MAX) {
                    sum_distancias += par.second;
                    nodos_alcanzables++;
                }
            }
        }
        else {
            // Usar Dijkstra
            // Validar que se haya entregado una funcion peso
            if (funcionPeso == nullptr) {
                throw std::invalid_argument("Se requiere funcionPeso para Closeness Centrality si el grafo es ponderado.");
            }
            auto distancias = GrafoUtilidades<VType, EType>::DijkstraShortestPath(G, u, funcionPeso);
            for (const auto& par : distancias) {
                if (par.second != INF && par.second != INT_MAX) {
                    sum_distancias += par.second;
                    nodos_alcanzables++;
                }
            }
        }

        // Calcular mediante Fórmula
        if (sum_distancias > 0.0 && num_vertices > 1) {
            double closeness_centralidad = (nodos_alcanzables - 1.0) / sum_distancias;

            if (!esConexo) {
                double s = (nodos_alcanzables - 1.0) / (num_vertices - 1.0);
                closeness_centralidad *= s;
            }

            return closeness_centralidad;
        }
        else {
            return 0.0;
        }
    }


public:

    /**
     * @brief Calcula la centralidad PageRank de los nodos.
     * @param G Referencia al Grafo.
     * @param d Factor de amortiguación (típicamente 0.85).
     * @param max_iter Límite de iteraciones para evitar ciclos infinitos.
     * @param tol Tolerancia para el criterio de convergencia.
     */
    static std::unordered_map<int, double> calcularPageRank(Grafo<VType, EType>& G, double d = 0.85, int max_iter = 100, double tol = 1e-6) {
        std::vector<int> nodos = G.vertices();
        int N = nodos.size();
        
        std::unordered_map<int, double> pr;
        if (N == 0) return pr;

        // 1. Inicialización: Todos los nodos empiezan con la misma probabilidad
        double initial_rank = 1.0 / N;
        for (int v : nodos) {
            pr[v] = initial_rank;
        }

        // 2. Ciclo iterativo
        for (int iter = 0; iter < max_iter; iter++) {
            std::unordered_map<int, double> next_pr;
            double sum_dangling = 0.0;

            // A. Aplicar la base del factor de amortiguación (teletransporte)
            double base_prob = (1.0 - d) / N;
            for (int v : nodos) {
                next_pr[v] = base_prob;
            }

            // B. Fase de "Push" (Empujar PageRank a los vecinos)
            for (int u : nodos) {
                std::vector<int> aristas_salida = G.incidentEdges(u);
                int out_degree = aristas_salida.size();

                if (out_degree > 0) {
                    // Dividimos el PageRank actual del nodo entre sus salidas
                    double pr_a_repartir = pr[u] / out_degree;
                    
                    for (int e : aristas_salida) {
                        int v = G.opposite(u, e); // El nodo destino
                        next_pr[v] += d * pr_a_repartir;
                    }
                } else {
                    // "Dangling node" (Nodo sin salidas): 
                    // Como la persona no puede hacer clic en otro enlace, asume que 
                    // salta aleatoriamente a cualquier otro nodo del grafo.
                    sum_dangling += d * (pr[u] / N);
                }
            }

            // C. Sumar el aporte de los dangling nodes y verificar convergencia
            double diferencia_total = 0.0;
            for (int v : nodos) {
                next_pr[v] += sum_dangling;
                diferencia_total += std::abs(next_pr[v] - pr[v]);
            }

            // Actualizar el estado
            pr = next_pr;

            // D. Criterio de parada
            if (diferencia_total < tol) {
                // Convergencia alcanzada antes del límite
                break; 
            }
        }
        return pr;
    }

    /**
     * @brief Calcula la centralidad de grado (Degree Centrality).
     * Mide el nivel de conexiones de un vértice normalizado por (N - 1).
     */
    static std::unordered_map<int, double> calcularDegreeCentrality(Grafo<VType, EType>& G) {
        std::vector<int> nodos = G.vertices();
        int N = nodos.size();
        
        std::unordered_map<int, double> degree_centrality;
        if (N <= 1) return degree_centrality;

        for (int u : nodos) {
            double grado = G.incidentEdges(u).size();
            degree_centrality[u] = grado / (N - 1.0);
        }

        return degree_centrality;
    }

    /**
     * @brief Calcula el Betweenness Centrality usando el Algoritmo de Brandes (pesado).
     * Mide cuántas veces un nodo aparece en el camino más corto entre otros dos nodos.
     *
     * @param funcionPeso Puntero a función normal para extraer el peso de la arista.
     */
    static std::unordered_map<int, double> calcularBetweennessCentrality(Grafo<VType, EType>& G, double (*funcionPeso)(EType)) {
        // Validar que se haya entregado una función de peso
        if (funcionPeso == nullptr) {
            throw std::invalid_argument("Se requiere funcionPeso para Betweenness Centrality.");
        }

        std::vector<int> nodos = G.vertices();
        std::unordered_map<int, double> betweenness;

        for (int v : nodos) {
            betweenness[v] = 0.0;
        }

        for (int s : nodos) {
            std::stack<int> S; 
            std::unordered_map<int, std::vector<int>> P; 
            std::unordered_map<int, double> sigma; 
            std::unordered_map<int, double> d; 

            for (int v : nodos) {
                sigma[v] = 0.0;
                d[v] = std::numeric_limits<double>::infinity();
            }

            sigma[s] = 1.0;
            d[s] = 0.0;

            std::priority_queue<std::pair<double, int>, 
                                std::vector<std::pair<double, int>>, 
                                std::greater<std::pair<double, int>>> Q;
            
            Q.push({0.0, s});

            while (!Q.empty()) {
                double dist_v = Q.top().first;
                int v = Q.top().second;
                Q.pop();

                if (dist_v > d[v]) continue;

                S.push(v);

                for (int e : G.incidentEdges(v)) {
                    int w = G.opposite(v, e);
                    double weight = funcionPeso(G.getEdgeElement(e)); 

                    if (d[w] > d[v] + weight) {
                        d[w] = d[v] + weight;
                        Q.push({d[w], w});
                        sigma[w] = sigma[v];
                        P[w].clear();
                        P[w].push_back(v);
                    } 
                    else if (d[w] == d[v] + weight) {
                        sigma[w] += sigma[v];
                        P[w].push_back(v);
                    }
                }
            }

            std::unordered_map<int, double> delta;
            for (int v : nodos) delta[v] = 0.0;

            while (!S.empty()) {
                int w = S.top();
                S.pop();
                
                for (int v : P[w]) {
                    if (sigma[w] != 0.0) { 
                        delta[v] += (sigma[v] / sigma[w]) * (1.0 + delta[w]);
                    }
                }
                if (w != s) {
                    betweenness[w] += delta[w];
                }
            }
        }

        if (!G.esDirigido()) {
            for (int v : nodos) {
                betweenness[v] /= 2.0;
            }
        }

        return betweenness;
    }

    /**
     * @brief Mide la centralidad de cercanía (Closeness Centrality) en la red.
     * @param G Referencia al grafo.
     * @param u Si es -1, se calcula Closeness Centrality para toda la red
     * @param esPonderado Si es true, usa Dijkstra para el camino más corto. Si es false, usa BFS.
     * @param esConexo Si es false, usa la variante de Wasserman and Faust para grafos con más de una componente conexa.
     * @param funcionPeso Puntero a función normal para extraer el peso de la arista.
     * @return std::unordered_map con el ID del vértice y su valor de centralidad.
     */
    static std::unordered_map<int, double> ClosenessCentrality(
        Grafo<VType, EType>& G,
        double (*funcionPeso)(EType) = nullptr,
        int u = -1,
        bool esPonderado = true,
        bool esConexo = true
    ) {
        std::unordered_map<int, double> closeness_centrality;
        int num_vertices = G.vertices().size();

        if (u == -1) {
            // Calculamos Closeness Centrality para todos los vértices del grafo.
            for (int v : G.vertices()) {
                closeness_centrality[v] = calcularClosenessCentrality(G, v, num_vertices, esPonderado, esConexo, funcionPeso);
            }
        }
        else {
            // Si no, calculamos solamente para el vértice "u"
            closeness_centrality[u] = calcularClosenessCentrality(G, u, num_vertices, esPonderado, esConexo, funcionPeso);
        }

        return closeness_centrality;
    }

    static void calcularAverageShortestPath() {

    }
};