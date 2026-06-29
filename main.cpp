#include <iostream>
#include <string>
#include <iomanip>
#include "grafoADT.h"
#include "redUtilidades.h"
#include "medidasCentralidad.h"

using namespace std;

// ==========================================
// FUNCIÓN AUXILIAR PARA RESUMIR RESULTADOS
// ==========================================
template <typename VType, typename EType>
void imprimirResumenMapa(const string& nombreMedida, const unordered_map<int, double>& mapa, Grafo<VType, EType>& G) {
    if (mapa.empty()) {
        cout << "  - " << nombreMedida << ": N/A (Grafo vacío)\n";
        return;
    }
    
    double max_val = 0.0;
    double min_val = 0.0;
    int max_nodo = -1;
    int min_nodo = -1;
    double suma = 0.0;
    bool primero = true;
    
    for (const auto& par : mapa) {
        suma += par.second;
        
        if (primero) {
            max_val = par.second;
            max_nodo = par.first;
            min_val = par.second;
            min_nodo = par.first;
            primero = false;
        } else {
            if (par.second > max_val) {
                max_val = par.second;
                max_nodo = par.first;
            }
            if (par.second < min_val) {
                min_val = par.second;
                min_nodo = par.first;
            }
        }
    }
    
    double promedio = suma / mapa.size();
    
    cout << "  - " << nombreMedida << ":\n";
    cout << "      Promedio = " << fixed << setprecision(6) << promedio << "\n";
    cout << "      Maximo(Nodo: " << G.getVertexElement(max_nodo) << ") = " << max_val << "\n";
    cout << "      Minimo(Nodo: " << G.getVertexElement(min_nodo) << ") = " << min_val << "\n";
}

// ==========================================
// FUNCIÓN PARA ANALIZAR UNA RED
// ==========================================
void analizarRed(const string& nombreRed, Grafo<string, AristaData>& G) {
    cout << "\n========================================================\n";
    cout << nombreRed << "\n";
    cout << "========================================================\n";
    cout << "- Total de Nodos  : " << G.numVertices() << "\n";
    cout << "- Total de Aristas: " << G.edges().size() << "\n";
    cout << "- Tipo de Grafo   : " << (G.esDirigido() ? "Dirigido" : "No Dirigido") << "\n\n";

    if (G.numVertices() == 0) {
        cout << "La red esta vacia. No se pueden calcular las medidas.\n";
        return;
    }

    // 1. Degree Centrality
    auto dc = AnalizadorCentralidad<string, AristaData>::calcularDegreeCentrality(G);
    imprimirResumenMapa("1. Degree Centrality", dc, G);

    // 2. Betweenness Centrality
    auto bc = AnalizadorCentralidad<string, AristaData>::calcularBetweennessCentrality(G, AristaData::getPesoMinimo);
    imprimirResumenMapa("2. Betweenness Centrality", bc, G);

    // 3. Closeness Centrality
    auto cc = AnalizadorCentralidad<string, AristaData>::calcularClosenessCentrality(G, -1, true, AristaData::getPesoMinimo);
    imprimirResumenMapa("3. Closeness Centrality", cc, G);

    // 4. PageRank
    auto pr = AnalizadorCentralidad<string, AristaData>::calcularPageRank(G);
    imprimirResumenMapa("4. PageRank", pr, G);

    // 5. Average Shortest Path Length
    double aspl = AnalizadorCentralidad<string, AristaData>::calcularAverageShortestPath(G, AristaData::getPesoMinimo);
    cout << "  - 5. Average Shortest Path Length:\n";
    cout << "      Valor = " << fixed << setprecision(6) << aspl << "\n";

    // 6. Eigenvector Centrality
    auto ec = AnalizadorCentralidad<string, AristaData>::calcularEigenvectorCentrality(G);
    imprimirResumenMapa("6. Eigenvector Centrality", ec, G);

    // 7. Clustering Coefficient (Manejo de excepciones para casos no implementados)
    try {
        unordered_map<int, double> cc_local;
        for (int v : G.vertices()) {
            // Se fuerza el envío del puntero a función (peso). Si el grafo es dirigido, lanzará la excepción.
            double c = AnalizadorCentralidad<string, AristaData>::calcularClustering(G, v, AristaData::getPesoMinimo);
            cc_local[v] = c;
        }
        imprimirResumenMapa("7. Clustering Coefficient", cc_local, G);
    } catch (const std::invalid_argument& e) {
        cout << "  - 7. Clustering Coefficient:\n";
        cout << "      [Excepcion] " << e.what() << "\n";
    } catch (const std::exception& e) {
        cout << "  - 7. Clustering Coefficient:\n";
        cout << "      [Error Inesperado] " << e.what() << "\n";
    }
}



int main() {
    // ==========================================
    // INSTANCIACIÓN DE LAS REDES
    // ==========================================
    Grafo<string, AristaData> redActores(false);       // false = no dirigido
    Grafo<string, AristaData> redIoT(true);            // true = dirigido
    Grafo<string, AristaData> redNetScience(false);    // false = no dirigido

    // ==========================================
    // CARGAR DATASETS
    // ==========================================
    cout << "\nCargando datasets...\n" << endl;

    // Selección de rutas para Windows y Linux
    #ifdef _WIN32
        std::string ruta_Actores = "..\\datasets\\imdb_edgelist.csv";
        std::string ruta_IoT = "..\\datasets\\train_test_network.csv";
        std::string ruta_NetScience = "..\\datasets\\NetScience.csv";
    #else
        // Sin el "../"
        string ruta_Actores = "datasets/imdb_edgelist.csv";
        string ruta_IoT = "datasets/train_test_network.csv";
        string ruta_NetScience = "datasets/NetScience.csv";
    #endif

    RedUtilidades::cargarDatasetCSV(ruta_Actores, redActores, "From", "To", "Strength");
    RedUtilidades::cargarDatasetCSV(ruta_IoT, redIoT, "src_ip", "dst_ip", "duration");
    RedUtilidades::cargarDatasetCSV(ruta_NetScience, redNetScience, "Source", "Target", "Weight");

    cout << "Carga completada. Iniciando analisis...\n";

    // ==========================================
    // EJECUTAR ANÁLISIS
    // ==========================================
    analizarRed("RED IoT", redIoT);
    analizarRed("RED NetScience", redNetScience);
    analizarRed("RED IMDb", redActores);

    return 0;
}