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

};

// CHEQUEAR SI ESTAN BIEN CONSTRUIDOS LOS GRAFOS 
    
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