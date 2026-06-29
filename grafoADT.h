#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>

// Plantilla: VType es el tipo de dato del Vértice, EType es el de la Arista
template <typename VType, typename EType>
class Grafo {
private:
    struct AristaInterna {
        int u; // Vértice origen
        int v; // Vértice destino
        EType elemento; // Peso, etiqueta, etc.
    };

    // Mapea el ID del vértice -> Elemento del vértice
    std::unordered_map<int, VType> mapa_vertices;
    
    // Mapea el ID de la arista -> Arista interna (u, v, elemento)
    std::unordered_map<int, AristaInterna> mapa_aristas;

    // Mapeo inverso de mapa_vertices (Permite buscar en O(1) si un vértice existe)
    std::unordered_map<VType, int> mapa_id_vertices;

    // LISTA DE ADYACENCIA: u -> (v -> id_arista)
    // Permite buscar vecinos y aristas en O(1) promedio
    std::unordered_map<int, std::unordered_map<int, int>> lista_adyacencia;

    // Contadores para generar IDs únicos
    int id_vertice_actual = 0;
    int id_arista_actual = 0;
    
    bool es_dirigido;

public:
    // Constructor
    Grafo(bool dirigido){
        es_dirigido = dirigido;
    }

    // ==========================================
    // MÉTODOS DE ACCESO
    // ==========================================
    bool esDirigido() const { 
        return es_dirigido; 
    }
    // Un arreglo (o par) con los dos puntos extremos de e
    std::pair<int, int> endVertices(int e) {
        if (mapa_aristas.find(e) == mapa_aristas.end()) throw std::invalid_argument("Arista no existe");
        return {mapa_aristas[e].u, mapa_aristas[e].v};
    }

    // El vértice opuesto de v en la arista e
    int opposite(int v, int e) {
        if (mapa_aristas.find(e) == mapa_aristas.end()) throw std::invalid_argument("Arista no existe");
        if (mapa_aristas[e].u == v) return mapa_aristas[e].v;
        if (mapa_aristas[e].v == v) return mapa_aristas[e].u;
        throw std::invalid_argument("El vértice no pertenece a la arista");
    }

    // v y w son adyacentes
    bool areAdjacent(int v, int w) {
        if (lista_adyacencia.find(v) != lista_adyacencia.end()) {
            if (lista_adyacencia[v].find(w) != lista_adyacencia[v].end()) return true;
        }
        if (!es_dirigido && lista_adyacencia.find(w) != lista_adyacencia.end()) {
            if (lista_adyacencia[w].find(v) != lista_adyacencia[w].end()) return true;
        }
        return false;
    }

    // Reemplaza el elemento en el vértice con x
    void replaceVertex(int v, VType x) {
        if (mapa_vertices.find(v) == mapa_vertices.end()) throw std::invalid_argument("Vértice no existe");
        mapa_vertices[v] = x;
    }

    // Reemplaza el elemento en la arista con x
    void replaceEdge(int e, EType x) {
        if (mapa_aristas.find(e) == mapa_aristas.end()) throw std::invalid_argument("Arista no existe");
        mapa_aristas[e].elemento = x;
    }

    // ==========================================
    // MÉTODOS DE ACTUALIZACIÓN
    // ==========================================

    // Inserta un vértice almacenando el elemento o. Retorna su ID (posición)
    int insertVertex(VType o) {
        // CASO 1: Si el vértice existe, simplemente retornamos su id
        if (mapa_id_vertices.find(o) != mapa_id_vertices.end()) {
            return mapa_id_vertices[o];
        }

        // CASO 2: Si no existe, lo creamos e insertamos
        int id = id_vertice_actual++;
        mapa_vertices[id] = o;
        mapa_id_vertices[o] = id;
        lista_adyacencia[id] = {}; // Inicializa su lista de adyacencia vacía
        return id;
    }

    // Inserta una arista (v,w) almacenando el elemento o. Retorna su ID (posición)
    int insertEdge(int v, int w, EType o) {
        if (mapa_vertices.find(v) == mapa_vertices.end() || mapa_vertices.find(w) == mapa_vertices.end()) {
            throw std::invalid_argument("Uno o ambos vértices no existen");
        }
        
        int id = id_arista_actual++;
        mapa_aristas[id] = {v, w, o};
        
        lista_adyacencia[v][w] = id;
        if (!es_dirigido) {
            lista_adyacencia[w][v] = id;
        }
        return id;
    }

    // Elimina el vértice (y sus aristas incidentes)
    void removeVertex(int v) {
        if (mapa_vertices.find(v) == mapa_vertices.end()) return;

        // Primero eliminamos todas las aristas conectadas a este vértice
        std::vector<int> incidentes = incidentEdges(v);
        for (int e : incidentes) {
            removeEdge(e);
        }

        // Luego eliminamos el vértice
        lista_adyacencia.erase(v);
        mapa_vertices.erase(v);
    }

    // Elimina la arista e
    void removeEdge(int e) {
        if (mapa_aristas.find(e) == mapa_aristas.end()) return;

        int u = mapa_aristas[e].u;
        int v = mapa_aristas[e].v;

        lista_adyacencia[u].erase(v);
        if (!es_dirigido) {
            lista_adyacencia[v].erase(u);
        }
        
        mapa_aristas.erase(e);
    }

    // ==========================================
    // MÉTODOS ITERADORES
    // ==========================================

    // Aristas incidentes a un vértice v
    std::vector<int> incidentEdges(int v) {
        std::vector<int> aristas;
        if (lista_adyacencia.find(v) != lista_adyacencia.end()) {
            for (const auto& par : lista_adyacencia[v]) {
                aristas.push_back(par.second); // par.second es el ID de la arista
            }
        }
        return aristas;
    }

    // Todos los vértices en el grafo
    std::vector<int> vertices() {
        std::vector<int> verts;
        for (const auto& par : mapa_vertices) {
            verts.push_back(par.first);
        }
        return verts;
    }

    // Todas las aristas en el grafo
    std::vector<int> edges() {
        std::vector<int> aristas_totales;
        for (const auto& par : mapa_aristas) {
            aristas_totales.push_back(par.first);
        }
        return aristas_totales;
    }

    // ==========================================
    // MÉTODOS PARA OBTENER DATOS
    // ==========================================

    VType getVertexElement(int v) {
        return mapa_vertices.at(v); 
    }

    EType getEdgeElement(int e) { 
        return mapa_aristas.at(e).elemento; 
    }

    /**
     *
     * Esto toma tiempo O(deg(v)) <- Existen formas más eficientes
     */
    int getEdgeID(int v, int w) {
        // Recorremos las aristas incidentes buscando si el vértice opuesto es w
        for (int e : incidentEdges(v)) {
            if (opposite(v, e) == w) {
                return e;
            }
        }
        return -1;
    }

    int numVertices() { 
        return mapa_vertices.size(); 
    }

    // ==========================================
    // ANÁLISIS DE MEMORIA
    // ==========================================
    size_t estimarMemoriaBytes() {
        size_t total = sizeof(*this); // Tamaño base de los punteros y variables de la clase

        // Un nodo en un std::unordered_map tiene un "overhead" (punteros al siguiente nodo y hash)
        // En sistemas de 64 bits, esto suele ser de unos 32 bytes adicionales por elemento.
        const size_t OVERHEAD_NODO = 32; 

        // 1. Memoria de mapa_vertices
        total += mapa_vertices.bucket_count() * sizeof(void*); 
        total += mapa_vertices.size() * (sizeof(int) + sizeof(VType) + OVERHEAD_NODO);

        // 2. Memoria de mapa_id_vertices
        total += mapa_id_vertices.bucket_count() * sizeof(void*);
        total += mapa_id_vertices.size() * (sizeof(VType) + sizeof(int) + OVERHEAD_NODO);

        // 3. Memoria de mapa_aristas
        total += mapa_aristas.bucket_count() * sizeof(void*);
        total += mapa_aristas.size() * (sizeof(int) + sizeof(AristaInterna) + OVERHEAD_NODO);

        // 4. Memoria de la lista_adyacencia (Un mapa que contiene otros mapas)
        total += lista_adyacencia.bucket_count() * sizeof(void*);
        total += lista_adyacencia.size() * (sizeof(int) + sizeof(std::unordered_map<int, int>) + OVERHEAD_NODO);
        
        for (const auto& par : lista_adyacencia) {
            total += par.second.bucket_count() * sizeof(void*);
            total += par.second.size() * (sizeof(int) + sizeof(int) + OVERHEAD_NODO);
        }

        return total;
    }
};