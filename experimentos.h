#pragma once

#include "grafoADT.h"
#include "rendimientoCentralidad.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <limits>
#include <algorithm>

template<typename VType, typename EType>
class ExperimentosSensibilidad {
private:
    // --- Utilidades para encontrar nodos estratégicos ---
    static int obtenerMax(const std::unordered_map<int, double>& mapa) {
        int id_max = -1;
        double max_val = -1.0;
        for (const auto& par : mapa) {
            if (par.second > max_val) {
                max_val = par.second;
                id_max = par.first;
            }
        }
        return id_max;
    }

    static int obtenerMin(const std::unordered_map<int, double>& mapa) {
        int id_min = -1;
        double min_val = std::numeric_limits<double>::max();
        for (const auto& par : mapa) {
            if (par.second < min_val) {
                min_val = par.second;
                id_min = par.first;
            }
        }
        return id_min;
    }

    static void imprimirCambio(const std::string& mensaje, double antes, double despues) {
        std::cout << "  - " << mensaje << "\n";
        std::cout << "    Valor ANTES   : " << std::fixed << std::setprecision(6) << antes << "\n";
        std::cout << "    Valor DESPUES : " << std::fixed << std::setprecision(6) << despues << "\n";
        double diff = despues - antes;
        std::cout << "    DIFERENCIA    : " << (diff >= 0 ? "+" : "") << diff << "\n\n";
    }

public:
    /**
     * @brief Ejecuta las pruebas de sensibilidad estratégica para las 7 métricas.
     * @param G Grafo a analizar.
     * @param funcionPeso Puntero a función de peso.
     * @param aristaDefault Objeto arista por defecto (ej. AristaData{1.0, 1.0}) para insertar.
     */
    static void ejecutarTodos(Grafo<VType, EType>& G, double (*funcionPeso)(EType), EType aristaDefault) {
        std::cout << "\n======================================================\n";
        std::cout << "   INICIANDO EXPERIMENTOS ESTRATEGICOS DE RED\n";
        std::cout << "======================================================\n\n";

        // Nota: Asumimos que los métodos de G.removeEdge(int id_arista) eliminan por ID.
        // Si tu ADT usa (u, v), reemplaza G.removeEdge(edge_id) por G.removeEdge(u, v).

        // ==========================================
        // 1. PAGERANK
        // ==========================================
        std::cout << "--- 1. PAGERANK ---\n";
        auto prOriginal = AnalizadorCentralidad<VType, EType>::calcularPageRank(G);
        int pr_max = obtenerMax(prOriginal);
        int pr_min = obtenerMin(prOriginal);

        std::cout << "[ESTRATEGIA ANIADIR]: Conectar el nodo mas importante (PR_MAX:" << pr_max << ") apuntando al menos importante (PR_MIN:" << pr_min << ").\n";
        if (pr_max != -1 && pr_min != -1 && pr_max != pr_min && G.getEdgeID(pr_max, pr_min) == -1) {
            G.insertEdge(pr_max, pr_min, aristaDefault);
            auto prAdd = AnalizadorCentralidad<VType, EType>::calcularPageRank(G);
            imprimirCambio("PageRank del nodo objetivo (deberia aumentar drasticamente)", prOriginal[pr_min], prAdd[pr_min]);
            G.removeEdge(G.getEdgeID(pr_max, pr_min)); // Revertir
        }

        std::cout << "[ESTRATEGIA QUITAR]: Quitar una conexion que apunte al nodo mas importante (PR_MAX:" << pr_max << ").\n";
        int edge_to_remove = -1, vecino_origen = -1;
        for (int v : G.vertices()) {
            int e = G.getEdgeID(v, pr_max);
            if (e != -1) { edge_to_remove = e; vecino_origen = v; break; }
        }
        if (edge_to_remove != -1) {
            EType backup = G.getEdgeElement(edge_to_remove);
            G.removeEdge(edge_to_remove);
            auto prDel = AnalizadorCentralidad<VType, EType>::calcularPageRank(G);
            imprimirCambio("PageRank del nodo mas importante (deberia bajar)", prOriginal[pr_max], prDel[pr_max]);
            G.insertEdge(vecino_origen, pr_max, backup); // Revertir
        }


        // ==========================================
        // 2. DEGREE CENTRALITY
        // ==========================================
        std::cout << "--- 2. DEGREE CENTRALITY ---\n";
        auto degOriginal = AnalizadorCentralidad<VType, EType>::calcularDegreeCentrality(G);
        int deg_min = obtenerMin(degOriginal);
        int deg_max = obtenerMax(degOriginal);

        std::cout << "[ESTRATEGIA ANIADIR]: Aniadir arista al nodo menos conectado (DEG_MIN:" << deg_min << ").\n";
        int otro_nodo = (deg_min == G.vertices()[0]) ? G.vertices()[1] : G.vertices()[0];
        if (G.getEdgeID(deg_min, otro_nodo) == -1) {
            G.insertEdge(deg_min, otro_nodo, aristaDefault);
            auto degAdd = AnalizadorCentralidad<VType, EType>::calcularDegreeCentrality(G);
            imprimirCambio("Degree Centrality (deberia subir 1/(N-1))", degOriginal[deg_min], degAdd[deg_min]);
            G.removeEdge(G.getEdgeID(deg_min, otro_nodo));
        }

        std::cout << "[ESTRATEGIA QUITAR]: Quitar una arista del nodo mas conectado (DEG_MAX:" << deg_max << ").\n";
        auto aristas_max = G.incidentEdges(deg_max);
        if (!aristas_max.empty()) {
            int e_del = aristas_max[0];
            int vecino = G.opposite(deg_max, e_del);
            EType backup = G.getEdgeElement(e_del);
            G.removeEdge(e_del);
            auto degDel = AnalizadorCentralidad<VType, EType>::calcularDegreeCentrality(G);
            imprimirCambio("Degree Centrality (deberia bajar)", degOriginal[deg_max], degDel[deg_max]);
            G.insertEdge(deg_max, vecino, backup);
        }


        // ==========================================
        // 3. BETWEENNESS CENTRALITY
        // ==========================================
        std::cout << "--- 3. BETWEENNESS CENTRALITY ---\n";
        auto bcOriginal = AnalizadorCentralidad<VType, EType>::calcularBetweennessCentrality(G, funcionPeso);
        int bc_max = obtenerMax(bcOriginal);

        std::cout << "[ESTRATEGIA ANIADIR]: Hacer un 'bypass' conectando dos vecinos desconectados del mayor puente (BC_MAX:" << bc_max << ").\n";
        auto vecinos_bc = G.incidentEdges(bc_max);
        int v1 = -1, v2 = -1;
        for (size_t i = 0; i < vecinos_bc.size(); ++i) {
            int n1 = G.opposite(bc_max, vecinos_bc[i]);
            for (size_t j = i + 1; j < vecinos_bc.size(); ++j) {
                int n2 = G.opposite(bc_max, vecinos_bc[j]);
                if (n1 != n2 && G.getEdgeID(n1, n2) == -1) {
                    v1 = n1; v2 = n2; break;
                }
            }
            if (v1 != -1) break;
        }
        if (v1 != -1 && v2 != -1) {
            G.insertEdge(v1, v2, aristaDefault);
            auto bcAdd = AnalizadorCentralidad<VType, EType>::calcularBetweennessCentrality(G, funcionPeso);
            imprimirCambio("Betweenness del puente (deberia bajar, el flujo ya no depende solo de el)", bcOriginal[bc_max], bcAdd[bc_max]);
            G.removeEdge(G.getEdgeID(v1, v2));
        }

        std::cout << "[ESTRATEGIA QUITAR]: Cortar una conexion clave del mayor puente (BC_MAX:" << bc_max << ").\n";
        if (!vecinos_bc.empty()) {
            int e_del = vecinos_bc[0];
            int vecino = G.opposite(bc_max, e_del);
            EType backup = G.getEdgeElement(e_del);
            G.removeEdge(e_del);
            auto bcDel = AnalizadorCentralidad<VType, EType>::calcularBetweennessCentrality(G, funcionPeso);
            imprimirCambio("Betweenness del puente (deberia bajar, se rompe un camino corto principal)", bcOriginal[bc_max], bcDel[bc_max]);
            G.insertEdge(bc_max, vecino, backup);
        }


        // ==========================================
        // 4. CLOSENESS CENTRALITY
        // ==========================================
        std::cout << "--- 4. CLOSENESS CENTRALITY ---\n";
        auto ccOriginal = AnalizadorCentralidad<VType, EType>::ClosenessCentrality(G, -1, true, funcionPeso);
        int cc_min = obtenerMin(ccOriginal);
        int cc_max = obtenerMax(ccOriginal);

        std::cout << "[ESTRATEGIA ANIADIR]: Conectar el nodo mas periferico (CC_MIN:" << cc_min << ") al mas central (CC_MAX:" << cc_max << ").\n";
        if (cc_min != -1 && cc_max != -1 && G.getEdgeID(cc_min, cc_max) == -1) {
            G.insertEdge(cc_min, cc_max, aristaDefault);
            auto ccAdd = AnalizadorCentralidad<VType, EType>::ClosenessCentrality(G, cc_min, true, funcionPeso);
            imprimirCambio("Closeness del nodo periferico (deberia subir al tener acceso rapido al centro)", ccOriginal[cc_min], ccAdd[cc_min]);
            G.removeEdge(G.getEdgeID(cc_min, cc_max));
        }

        std::cout << "[ESTRATEGIA QUITAR]: Quitar una conexion del nodo mas central (CC_MAX:" << cc_max << ").\n";
        auto vecinos_cc = G.incidentEdges(cc_max);
        if (!vecinos_cc.empty()) {
            int e_del = vecinos_cc[0];
            int vecino = G.opposite(cc_max, e_del);
            EType backup = G.getEdgeElement(e_del);
            G.removeEdge(e_del);
            auto ccDel = AnalizadorCentralidad<VType, EType>::ClosenessCentrality(G, cc_max, true, funcionPeso);
            imprimirCambio("Closeness del nodo central (deberia bajar al perder un acceso directo)", ccOriginal[cc_max], ccDel[cc_max]);
            G.insertEdge(cc_max, vecino, backup);
        }


        // ==========================================
        // 5. AVERAGE SHORTEST PATH
        // ==========================================
        std::cout << "--- 5. AVERAGE SHORTEST PATH ---\n";
        double aspOriginal = AnalizadorCentralidad<VType, EType>::AverageShortestPath(G, funcionPeso);

        std::cout << "[ESTRATEGIA ANIADIR]: Unir nodos aleatorios para compactar la red.\n";
        if (pr_min != -1 && pr_max != -1 && G.getEdgeID(pr_min, pr_max) == -1) {
            G.insertEdge(pr_min, pr_max, aristaDefault);
            double aspAdd = AnalizadorCentralidad<VType, EType>::AverageShortestPath(G, funcionPeso);
            imprimirCambio("Average Shortest Path (deberia bajar, red mas compacta)", aspOriginal, aspAdd);
            G.removeEdge(G.getEdgeID(pr_min, pr_max));
        }

        std::cout << "[ESTRATEGIA QUITAR]: Quitar arista clave para alargar caminos.\n";
        if (bc_max != -1 && !G.incidentEdges(bc_max).empty()) {
            int e_del = G.incidentEdges(bc_max)[0];
            int vecino = G.opposite(bc_max, e_del);
            EType backup = G.getEdgeElement(e_del);
            G.removeEdge(e_del);
            double aspDel = AnalizadorCentralidad<VType, EType>::AverageShortestPath(G, funcionPeso);
            imprimirCambio("Average Shortest Path (deberia subir, caminos mas largos)", aspOriginal, aspDel);
            G.insertEdge(bc_max, vecino, backup);
        }


        // ==========================================
        // 6. CLUSTERING COEFFICIENT
        // ==========================================
        std::cout << "--- 6. CLUSTERING COEFFICIENT ---\n";
        // Buscamos un nodo con al menos 2 vecinos
        int cl_node = -1;
        for (int v : G.vertices()) {
            if (G.incidentEdges(v).size() >= 2) { cl_node = v; break; }
        }

        if (cl_node != -1) {
            double clOriginal = AnalizadorCentralidad<VType, EType>::Clustering(G, cl_node, funcionPeso);
            std::cout << "[ESTRATEGIA ANIADIR]: Conectar a dos vecinos de Nodo " << cl_node << " para formar un triangulo.\n";
            
            auto vecinos_cl = G.incidentEdges(cl_node);
            int v1 = -1, v2 = -1;
            for (size_t i = 0; i < vecinos_cl.size(); ++i) {
                int n1 = G.opposite(cl_node, vecinos_cl[i]);
                for (size_t j = i + 1; j < vecinos_cl.size(); ++j) {
                    int n2 = G.opposite(cl_node, vecinos_cl[j]);
                    if (G.getEdgeID(n1, n2) == -1) { v1 = n1; v2 = n2; break; }
                }
                if (v1 != -1) break;
            }

            if (v1 != -1 && v2 != -1) {
                G.insertEdge(v1, v2, aristaDefault);
                double clAdd = AnalizadorCentralidad<VType, EType>::Clustering(G, cl_node, funcionPeso);
                imprimirCambio("Clustering Local (deberia subir, se formo un grupo cerrado)", clOriginal, clAdd);
                G.removeEdge(G.getEdgeID(v1, v2));
            } else {
                std::cout << "  No se encontraron vecinos desconectados (Clustering ya es 1.0).\n\n";
            }
            
            // Para quitar, buscamos vecinos que SÍ estén conectados
            std::cout << "[ESTRATEGIA QUITAR]: Desconectar dos vecinos del Nodo " << cl_node << " para romper un triangulo.\n";
            v1 = -1; v2 = -1;
            int edge_between_neighbors = -1;
            for (size_t i = 0; i < vecinos_cl.size(); ++i) {
                int n1 = G.opposite(cl_node, vecinos_cl[i]);
                for (size_t j = i + 1; j < vecinos_cl.size(); ++j) {
                    int n2 = G.opposite(cl_node, vecinos_cl[j]);
                    edge_between_neighbors = G.getEdgeID(n1, n2);
                    if (edge_between_neighbors != -1) { v1 = n1; v2 = n2; break; }
                }
                if (v1 != -1) break;
            }

            if (v1 != -1 && v2 != -1 && edge_between_neighbors != -1) {
                EType backup = G.getEdgeElement(edge_between_neighbors);
                G.removeEdge(edge_between_neighbors);
                double clDel = AnalizadorCentralidad<VType, EType>::Clustering(G, cl_node, funcionPeso);
                imprimirCambio("Clustering Local (deberia bajar, se rompio la pandilla)", clOriginal, clDel);
                G.insertEdge(v1, v2, backup);
            } else {
                std::cout << "  No se encontraron vecinos conectados (Clustering ya es 0.0).\n\n";
            }
        }


        // ==========================================
        // 7. EIGENVECTOR CENTRALITY
        // ==========================================
        std::cout << "--- 7. EIGENVECTOR CENTRALITY ---\n";
        auto evOriginal = AnalizadorCentralidad<VType, EType>::calcularEigenvectorCentrality(G);
        int ev_max = obtenerMax(evOriginal);
        int ev_min = obtenerMin(evOriginal);

        std::cout << "[ESTRATEGIA ANIADIR]: Conectar el nodo mas irrelevante (EV_MIN:" << ev_min << ") al lider (EV_MAX:" << ev_max << ").\n";
        if (ev_min != -1 && ev_max != -1 && ev_max != ev_min && G.getEdgeID(ev_min, ev_max) == -1) {
            G.insertEdge(ev_min, ev_max, aristaDefault);
            auto evAdd = AnalizadorCentralidad<VType, EType>::calcularEigenvectorCentrality(G);
            imprimirCambio("Eigenvector de EV_MIN (deberia subir drasticamente por influencia del lider)", evOriginal[ev_min], evAdd[ev_min]);
            G.removeEdge(G.getEdgeID(ev_min, ev_max));
        }

        std::cout << "[ESTRATEGIA QUITAR]: Quitarle un seguidor al lider (EV_MAX:" << ev_max << ").\n";
        auto vecinos_ev = G.incidentEdges(ev_max);
        if (!vecinos_ev.empty()) {
            int e_del = vecinos_ev[0];
            int vecino = G.opposite(ev_max, e_del);
            EType backup = G.getEdgeElement(e_del);
            G.removeEdge(e_del);
            auto evDel = AnalizadorCentralidad<VType, EType>::calcularEigenvectorCentrality(G);
            imprimirCambio("Eigenvector del lider (deberia bajar al perder conexiones)", evOriginal[ev_max], evDel[ev_max]);
            G.insertEdge(ev_max, vecino, backup);
        }
        
        std::cout << "======================================================\n";
        std::cout << "       EXPERIMENTOS FINALIZADOS EXITOSAMENTE\n";
        std::cout << "======================================================\n";
    }
};