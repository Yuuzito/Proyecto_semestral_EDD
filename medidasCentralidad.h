#pragma once
#include "GrafoRed.h"
#include <unordered_map>
#include <vector>
#include <cmath> // Para std::abs

template<typename VType>
class AnalizadorCentralidad {
public:

    /**
     * @brief Calcula la centralidad PageRank de los nodos.
     * @param red Referencia al GrafoRed.
     * @param d Factor de amortiguación (típicamente 0.85).
     * @param max_iter Límite de iteraciones para evitar ciclos infinitos.
     * @param tol Tolerancia para el criterio de convergencia.
     */
    static std::unordered_map<int, double> calcularPageRank(GrafoRed<VType>& red, double d = 0.85, int max_iter = 100, double tol = 1e-6) {
        auto& grafo = red.getGrafo();
        std::vector<int> nodos = grafo.vertices();
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
                std::vector<int> aristas_salida = grafo.incidentEdges(u);
                int out_degree = aristas_salida.size();

                if (out_degree > 0) {
                    // Dividimos el PageRank actual del nodo entre sus salidas
                    double pr_a_repartir = pr[u] / out_degree;
                    
                    for (int e : aristas_salida) {
                        int v = grafo.opposite(u, e); // El nodo destino
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
};