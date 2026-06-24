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
     * @brief Mide la Centralidad de Cercanía (Closeness Centrality). Se puede calcular para toda la red o para un vértice específico.
     *
     * @param G Referencia al grafo.
     * @param u Si es -1, se calcula Closeness Centrality para toda la red
     * @param esConexo Booleano que indica si el grafo es conexo (true) o no es conexo (false: usa la variante de Wasserman y Faust para componentes no conexas).
     * @param funcionPeso Puntero a función para extraer el peso de la arista (solo si es ponderado).
     *
     * @return unordered_map con el ID del vértice y su valor de centralidad.
     */
    static std::unordered_map<int, double> ClosenessCentrality(
        Grafo<VType, EType>& G,
        int u = -1,
        bool esConexo = true,
        double (*funcionPeso)(EType) = nullptr
    ) {

        std::unordered_map<int, double> closeness_centrality;
        int num_vertices = G.vertices().size();

        // Determinar qué vértices procesar
        std::vector<int> vertices_a_procesar;
        if (u == -1) {
            // Todos los vértices del grafo
            vertices_a_procesar = G.vertices();
        }
        else {
            // El vértice u recibido como parametro
            vertices_a_procesar.push_back(u);
        }

        for (int v : vertices_a_procesar) {
            double suma_distancias = 0.0;
            int nodos_alcanzables = 0;
            std::unordered_map<int, double> distancias;

            if (funcionPeso == nullptr) {
                // Usar BFS
                distancias = GrafoUtilidades<VType, EType>::BFS(G, v);
            }
            else {
                // Usar Dijkstra
                distancias = GrafoUtilidades<VType, EType>::DijkstraShortestPath(G, v, funcionPeso);
            }

            // Calcular la suma de las distancias y nodos alcanzables
            for (const auto& par : distancias) {
                if (par.second != INF) {
                    suma_distancias += par.second;
                    nodos_alcanzables++;
                }
            }

            // Aplicamos Fórmula
            if (suma_distancias > 0.0 && num_vertices > 1) {
                // Fórmula estándar (Nodos alcanzables / la suma de distancias)
                double f_centrality = (nodos_alcanzables - 1.0) / suma_distancias;

                // Normalización de Wasserman and Faust para grafos no conexos
                if (!esConexo) {
                    double s = (nodos_alcanzables - 1.0) / (num_vertices - 1.0);
                    f_centrality *= s;
                }

                closeness_centrality[v] = f_centrality;
            } else {
                closeness_centrality[v] = 0.0;
            }
        }
    
        return closeness_centrality;
    }

    /**
     * @brief Mide la Longitud Promedio del Camino Más Corto (Average Shortest Path Length). Se calcula sobre todos los pares de nodos alcanzables en la red.
     *
     * @param G Referencia al grafo.
     * @param funcionPeso Puntero a función para extraer el peso de la arista (solo si es ponderado).
     *
     * @return double El promedio de las distancias más cortas válidas.
     */
    static double AverageShortestPath(Grafo<VType, EType>& G, double (*funcionPeso)(EType) = nullptr) {

        int num_vertices = G.vertices().size();
        // Si hay 1 o 0 nodos, no hay caminos posibles
        if (num_vertices <= 1) return 0.0;

        double suma_total_distancias = 0.0;
        long long pares_validos = 0;
        int procesados = 0;

        for (int i : G.vertices()) {
            procesados++;
            // Imprime cada 100 nodos. \r hace que se sobreescriba la misma línea.
            if (procesados % 100 == 0) {
                std::cout << "Procesando nodo " << procesados << " de " << num_vertices << "\r" << std::flush;
            }

            std::unordered_map<int, double> distancias;

            if (funcionPeso == nullptr) {
                // Usar BFS
                distancias = GrafoUtilidades<VType, EType>::BFS(G, i);
            }
            else {
                // Usar Dijkstra
                distancias = GrafoUtilidades<VType, EType>::DijkstraShortestPath(G, i, funcionPeso);
            }

            for (const auto& par : distancias) {
                int j = par.first;
                double distancia = par.second;

                if (i != j && distancia != INF) {
                    suma_total_distancias += distancia;
                    pares_validos++;
                }
            }
        }

        // Si el grafo no tiene aristas o está totalmente desconectado
        if (pares_validos == 0) return 0.0;

        // Fórmula: SUMATORIA (distancia mínima entre el nodo i y el nodo j) / N(N-1)
        // Retornamos la sumatoria dividida por los caminos que realmente existían (Esto para grafos conexos y no conexos)
        return suma_total_distancias / pares_validos;
    }
};