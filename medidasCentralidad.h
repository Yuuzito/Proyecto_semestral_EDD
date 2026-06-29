#pragma once

#include "grafoADT.h"
#include "grafoUtilidades.h"
#include <unordered_map>
#include <vector>
#include <cmath>
#include <queue>   
#include <stack>   
#include <limits>

template<typename VType, typename EType>
class AnalizadorCentralidad {
public:
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
    static std::unordered_map<int, double> calcularClosenessCentrality(
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

        // Inicialización: Todos los nodos empiezan con la misma probabilidad
        double initial_rank = 1.0 / N;
        for (int v : nodos) {
            pr[v] = initial_rank;
        }

        // Ciclo iterativo
        for (int iter = 0; iter < max_iter; iter++) {
            std::unordered_map<int, double> next_pr;
            double sum_dangling = 0.0;

            // Aplicar la base del factor de amortiguación (teletransporte)
            double base_prob = (1.0 - d) / N;
            for (int v : nodos) {
                next_pr[v] = base_prob;
            }

            // Fase de "Push" (Empujar PageRank a los vecinos)
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
                    // Nodo sin salidas: 
                    // Como la persona no puede hacer clic en otro enlace, asume que 
                    // salta aleatoriamente a cualquier otro nodo del grafo.
                    sum_dangling += d * (pr[u] / N);
                }
            }

            // Sumar el aporte de los nodos sin salidas y verificar convergencia
            double diferencia_total = 0.0;
            for (int v : nodos) {
                next_pr[v] += sum_dangling;
                diferencia_total += std::abs(next_pr[v] - pr[v]);
            }

            // Actualizar el estado
            pr = next_pr;

            // Criterio de parada
            if (diferencia_total < tol) {
                // Convergencia alcanzada antes del límite
                break; 
            }
        }
        return pr;
    }



    /**
     * @brief Mide la Longitud Promedio del Camino Más Corto (Average Shortest Path Length). Se calcula sobre todos los pares de nodos alcanzables en la red.
     *
     * @param G Referencia al grafo.
     * @param funcionPeso Puntero a función para extraer el peso de la arista (solo si es ponderado).
     *
     * @return double El promedio de las distancias más cortas válidas.
     */
    static double calcularAverageShortestPath(Grafo<VType, EType>& G, double (*funcionPeso)(EType) = nullptr) {

        int num_vertices = G.vertices().size();
        // Si hay 1 o 0 nodos, no hay caminos posibles
        if (num_vertices <= 1) return 0.0;

        double suma_total_distancias = 0.0;
        long long pares_validos = 0;

        for (int i : G.vertices()) {
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



    /**
     * @brief Calcula la Centralidad de Vector Propio (Eigenvector Centrality).
     * 
     * @param red Referencia al GrafoRed.
     * @param max_iter Límite de iteraciones.
     * @param tol Tolerancia para el criterio de convergencia.
     * 
     * @return std::unordered_map<int, double> Mapa con el ID del vértice y su valor de centralidad.
     */
    static std::unordered_map<int, double> calcularEigenvectorCentrality(Grafo<VType, EType>& grafo, int max_iter = 100, double tol = 1e-6) {
        std::vector<int> nodos = grafo.vertices();
        int N = nodos.size();
        if (N == 0) return {};

        std::unordered_map<int, double> eigenvector;
        for (int v : nodos) eigenvector[v] = 1.0;

        for (int i = 0; i < max_iter; i++) {
            std::unordered_map<int, double> next_eigenvector;

            // --- CAMBIO CLAVE: AÑADIR BUCLE PROPIO ---
            // Esto inicializa cada nodo con su propio valor actual (Equivalente a sumar la Matriz Identidad I)
            for (int v : nodos) {
                next_eigenvector[v] = eigenvector[v];
            }

            // FASE PUSH: Acumulamos la influencia de los vecinos
            for (int u : nodos) {
                for (int e : grafo.incidentEdges(u)) {
                    int v = grafo.opposite(u, e);
                    double peso = static_cast<double>(grafo.getEdgeElement(e));
                    next_eigenvector[v] += eigenvector[u] * peso;
                }
            }

            // CALCULO NORMA L2
            double suma_cuadrados = 0.0;
            for (int v : nodos) suma_cuadrados += (next_eigenvector[v] * next_eigenvector[v]);
            double norma = std::sqrt(suma_cuadrados);

            if (norma == 0) break;

            // NORMALIZACIÓN
            double diferencia_total = 0.0;
            for (int v : nodos) {
                double valor_normalizado = next_eigenvector[v] / norma;
                diferencia_total += std::abs(valor_normalizado - eigenvector[v]);
                next_eigenvector[v] = valor_normalizado;
            }

            eigenvector = next_eigenvector;

            if (diferencia_total < tol) break;
        }
        return eigenvector;
    }


    
    /**
     * @brief Calcula el Coeficiente de Clustering Local (para un nodo específico).
     *
     * @param G Referencia al grafo.
     * @param u ID del vértice a evaluar.
     * @param funcionPeso Puntero a función para extraer el peso de la arista (solo si es ponderado).
     *
     * @return double Coeficiente de Clustering (entre 0.0 y 1.0).
     */
    static double calcularClustering(Grafo<VType, EType>& G, int u, double (*funcionPeso)(EType) = nullptr) {
        double C_u;

        // ==========================================
        // Si el grafo no es dirigido
        // ==========================================
        if (!G.esDirigido()) {
            double deg_u = G.incidentEdges(u).size();
            if (deg_u < 2.0) return 0.0;

            // ==========================================
            // NO PONDERADO
            // ==========================================
            if (funcionPeso == nullptr) {
                std::vector<int> vecinos;
                for (int e : G.incidentEdges(u)) {
                    vecinos.push_back(G.opposite(u, e));
                }

                // Calcular T(u): Triángulos no dirigidos
                double T_u = GrafoUtilidades<VType, EType>::contarTriangulos(G, u, vecinos);

                // Fórmula: C_u = 2 * T(u) / (deg(u) * (deg(u) - 1))
                C_u = (2.0 * T_u) / (deg_u * (deg_u - 1));
            }
            // ==========================================
            // PONDERADO
            // ==========================================
            else {
                // Obtener peso máximo en todo el grafo para normalizar
                double max_w = GrafoUtilidades<VType, EType>::getPesoMaximo(G, funcionPeso);
                if (max_w == 0.0) return 0.0;

                // Obtener los vecinos de "u" junto con el ID de la arista que los conecta
                std::vector<std::pair<int, int>> vecinos; // {id_vecino, id_arista_con_u}
                for (int e : G.incidentEdges(u)) {
                    vecinos.push_back({G.opposite(u, e), e});
                }

                double suma_pesos = 0.0;
                
                // Calcular T(u): Media geométrica de pesos de los triángulos
                for (int i = 0; i < vecinos.size(); ++i) {

                    int v = vecinos[i].first;
                    int edge_uv = vecinos[i].second;
                    double w_uv = funcionPeso(G.getEdgeElement(edge_uv)) / max_w; // Peso normalizado

                    for (int j = i + 1; j < vecinos.size(); ++j) {
                        int w = vecinos[j].first;
                        int edge_uw = vecinos[j].second;

                        // Verificar si los vecinos v y w están conectados entre sí
                        int edge_vw = G.getEdgeID(v, w);
                        if (edge_vw != -1) {
                            double w_uw = funcionPeso(G.getEdgeElement(edge_uw)) / max_w;
                            double w_vw = funcionPeso(G.getEdgeElement(edge_vw)) / max_w;

                            // Promedio geométrico de los tres pesos normalizados del triángulo
                            suma_pesos += std::cbrt(w_uv * w_uw * w_vw);
                        }
                    }
                }

                // Fórmula: C_u = 2 * T(u) / (deg(u) * (deg(u) - 1))
                C_u = (2.0 * suma_pesos) / (deg_u * (deg_u - 1.0));
            }
        }
        // ==========================================
        // Si el grafo es dirigido
        // ==========================================
        else {
            // ==========================================
            // NO PONDERADO
            // ==========================================
            if (funcionPeso == nullptr) {
                // Obtener todos los vecinos únicos de "u" (tanto entrantes como salientes)
                std::vector<int> vecinos;
                for (int v : G.vertices()) {
                    if (v == u) continue;

                    if (G.areAdjacent(u, v) || G.areAdjacent(v, u)) {
                        vecinos.push_back(v);
                    }
                }

                // Calcular Grado Total (in + out) y Grado Recíproco (conexiones bidireccionales u <-> v)
                double deg_total = 0.0;
                double deg_reciproco = 0.0;

                for (int v : vecinos) {
                    bool sale = G.areAdjacent(u, v);    // u -> v
                    bool entra = G.areAdjacent(v, u);   // v -> u

                    if (sale) deg_total += 1.0;
                    if (entra) deg_total += 1.0;
                    if (sale && entra) deg_reciproco += 1.0;
                }

                // Calcular el total de triángulos dirigidos posibles
                // Fórmula del denominador: 2 * (deg_tot * (deg_tot - 1) - 2 * deg_recip)
                double denominador = 2.0 * (deg_total * (deg_total - 1.0) - 2.0 * deg_reciproco);
                if (denominador == 0.0) return 0.0;

                // Calcular T(u): Triángulos dirigidos
                double T_u = GrafoUtilidades<VType, EType>::contarTriangulosDirigidos(G, u, vecinos);
                
                // Fórmula final: C_u = T(u) / denominador
                C_u = T_u / denominador;
            }
            else {
                // Caso ponderado no cubierto
                throw std::invalid_argument("El cálculo del Clustering para grafos dirigidos y ponderados no está implementado.");
            }
        }

        return C_u;
    }
};