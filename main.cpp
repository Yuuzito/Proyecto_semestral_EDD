#include <iostream>
#include "grafoADT.h"
#include "lecturaDataSet.cpp"

#include <chrono>
#include "extern/csv.h"

template <typename V, typename E>
void loadDatasetIMDb(const std::string& fileName, Grafo<V, E>& grafo) {
    try {
        // Configuramos el parser indicando que hay 3 columnas
        io::CSVReader<3> in(fileName);
        
        // Leemos la cabecera exacta de tu archivo
        in.read_header(io::ignore_extra_column, "From", "To", "Strength");

        std::string from;
        std::string to;
        double strength; // El parser manejará la conversión de "1" a double

        // Diccionario auxiliar: Mapea el Nombre (string) -> ID interno del Grafo (int)
        // Esto evita tener que buscar linealmente en el grafo entero por cada iteración.
        // NOTA: esto deberia ser implementado en el ADT en caso de implementar el uso de la libreria
        std::unordered_map<std::string, int> nameToId;

        //cout << "Cargando dataset desde: " << fileName << "..." << endl;

        // Leemos fila por fila
        while (in.read_row(from, to, strength)) {
            // Evaluamos e inicializamos id_v1
            int id_v1 = nameToId.count(from) ? nameToId[from] : (nameToId[from] = grafo.insertVertex(from));
            // Evaluamos e inicializamos id_v2
            int id_v2 = nameToId.count(to) ? nameToId[to] : (nameToId[to] = grafo.insertVertex(to));
            E peso_arista = static_cast<E>(strength);

            // Insertamos la arista en el grafo
            grafo.insertEdge(id_v1, id_v2, peso_arista);
        }

        //cout << "Carga completada exitosamente." << endl;
    }
    catch(const std::exception& e) {
        std::cerr << "Error al leer el archivo CSV: " << e.what() << std::endl;
    }
}


template <typename V, typename E>
void loadDatasetIoT(const std::string& fileName, Grafo<V, E>& grafo) {
    try {
        // Le indicamos que solo extraeremos 3 columnas, sin importar cuántas tenga el archivo real
        io::CSVReader<3> in(fileName);
        
        // Mapeamos los nombres exactos de las columnas que nos interesan
        in.read_header(io::ignore_extra_column, "src_ip", "dst_ip", "duration");

        std::string src_ip;
        std::string dst_ip;
        std::string duration_str; // Lo leemos como string para manejar los guiones "-"

        std::unordered_map<std::string, int> nameToId;

        while (in.read_row(src_ip, dst_ip, duration_str)) {
            // Evaluamos e inicializamos id_v1
            int id_v1 = nameToId.count(src_ip) ? nameToId[src_ip] : (nameToId[src_ip] = grafo.insertVertex(src_ip));
            // Evaluamos e inicializamos id_v2
            int id_v2 = nameToId.count(dst_ip) ? nameToId[dst_ip] : (nameToId[dst_ip] = grafo.insertVertex(dst_ip));
            
            // Procesamos el peso (duration), manejando el caso de error o guión
            double strength = 0.0;
            if (duration_str != "-") {
                try {
                    strength = std::stod(duration_str);
                } catch (...) {
                    strength = 0.0;
                }
            }
            
            E peso_arista = static_cast<E>(strength);

            // Insertamos la arista en el grafo
            grafo.insertEdge(id_v1, id_v2, peso_arista);
        }
    }
    catch(const std::exception& e) {
        std::cerr << "Error al leer el archivo CSV: " << e.what() << std::endl;
    }
}


// ---------------------------------------------------------
// FUNCION PRINCIPAL DE BENCHMARKING
// ---------------------------------------------------------
int main() {
    std::cout << "=================================================\n";
    std::cout << "       INICIANDO BENCHMARK DE LECTURA CSV        \n";
    std::cout << "=================================================\n\n";

    // --- PARSERS ORIGINALES ---
    auto parser_imdb = [](const std::vector<std::string>& cols, std::string& src, std::string& dst, int& peso) {
        if (cols.size() < 3) return false; 
        src = cols[0]; dst = cols[1]; 
        try { peso = std::stoi(cols[2]); return true; } catch (...) { return false; }
    };

    auto parser_train_test = [](const std::vector<std::string>& columnas, std::string& origen, std::string& destino, double& peso) {
        if (columnas.size() < 7) return false; 
        origen = columnas[0]; destino = columnas[2];  
        try { peso = std::stod(columnas[6]); } catch (...) { peso = 0.0; }
        return true; 
    };


    // =========================================================
    // TEST 1: DATASET IMDB (imdb_edgelist.csv)
    // =========================================================
    std::cout << "--- TEST 1: DATASET IMDb ---\n";
    
    // 1A: Metodo Original
    Grafo<std::string, int> grafo_imdb_original(false);
    std::cout << "[Original] Procesando...\n";
    auto start_imdb_orig = std::chrono::high_resolution_clock::now();
    cargarDatasetGeneral(grafo_imdb_original, "datasets/imdb_edgelist.csv", true, parser_imdb);
    auto end_imdb_orig = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> t_imdb_orig = end_imdb_orig - start_imdb_orig;
    std::cout << "   -> Nodos creados: " << grafo_imdb_original.numVertices() << "\n";
    std::cout << "   -> Tiempo: " << t_imdb_orig.count() << " ms\n\n";

    // 1B: Nuevo Metodo (csv.h)
    Grafo<std::string, int> grafo_imdb_nuevo(false);
    std::cout << "[Nuevo csv.h] Procesando...\n";
    auto start_imdb_nuevo = std::chrono::high_resolution_clock::now();
    loadDatasetIMDb("datasets/imdb_edgelist.csv", grafo_imdb_nuevo);
    auto end_imdb_nuevo = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> t_imdb_nuevo = end_imdb_nuevo - start_imdb_nuevo;
    std::cout << "   -> Nodos creados: " << grafo_imdb_nuevo.numVertices() << "\n";
    std::cout << "   -> Tiempo: " << t_imdb_nuevo.count() << " ms\n\n";


    // =========================================================
    // TEST 2: DATASET IOT (train_test_network.csv)
    // =========================================================
    std::cout << "--- TEST 2: DATASET IoT ---\n";

    // 2A: Metodo Original
    Grafo<std::string, double> grafo_iot_original(true);
    std::cout << "[Original] Procesando...\n";
    auto start_iot_orig = std::chrono::high_resolution_clock::now();
    cargarDatasetGeneral(grafo_iot_original, "datasets/train_test_network.csv", true, parser_train_test);
    auto end_iot_orig = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> t_iot_orig = end_iot_orig - start_iot_orig;
    std::cout << "   -> Nodos creados: " << grafo_iot_original.numVertices() << "\n";
    std::cout << "   -> Tiempo: " << t_iot_orig.count() << " ms\n\n";

    // 2B: Nuevo Metodo (csv.h)
    Grafo<std::string, double> grafo_iot_nuevo(true);
    std::cout << "[Nuevo csv.h] Procesando...\n";
    auto start_iot_nuevo = std::chrono::high_resolution_clock::now();
    loadDatasetIoT("datasets/train_test_network.csv", grafo_iot_nuevo);
    auto end_iot_nuevo = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> t_iot_nuevo = end_iot_nuevo - start_iot_nuevo;
    std::cout << "   -> Nodos creados: " << grafo_iot_nuevo.numVertices() << "\n";
    std::cout << "   -> Tiempo: " << t_iot_nuevo.count() << " ms\n\n";

    std::cout << "=================================================\n";
    std::cout << "                FIN DEL BENCHMARK                \n";
    std::cout << "=================================================\n";

    return 0;
}

// int main() {
//     Grafo<std::string, int> red_actores(false); // false = no dirigido.
    
//     Funcion de lectura para el dataset de IMDb. Extrae Actor1, Actor2 y Cantidad de Colaboraciones (peso).
//     auto parser_imdb = [](const std::vector<std::string>& cols, std::string& src, std::string& dst, int& peso) {
//         if (cols.size() < 3) return false; // Ignorar líneas rotas.
        
//         src = cols[0]; // Actor 1
//         dst = cols[1]; // Actor 2
//         try {
//             peso = std::stoi(cols[2]); // Convertir texto a Int.
//             return true;
//         } catch (...) { return false; }
//     };

//     std::cout << "Cargando IMDb...\n";
//     cargarDatasetGeneral(red_actores, "..\\datasets\\imdb_edgelist.csv", true, parser_imdb);

//     Grafo<std::string, double> red_iot(true); // true = dirigido.

//     Funcion de lectura para el dataset de IoT. Extrae src_ip, dst_ip y duration (peso).
//     auto parser_train_test = [](const std::vector<std::string>& columnas, std::string& origen, std::string& destino, double& peso) {
        
//         Verificamos que la fila tenga al menos 7 elementos.
//         if (columnas.size() < 7) {
//             return false; 
//         }
        
//         origen  = columnas[0];  // Extraemos src_ip.
//         destino = columnas[2];  // Extraemos dst_ip.
//         try {
//             peso = std::stod(columnas[6]); // Extraemos duration y lo pasamos a decimal.
//         } catch (...) {
//             Si el texto de duration viene con un guión "-" o un error, le ponemos 0.
//             peso = 0.0; 
//         }

//         return true; // Si es valida creamos la arista.
//     };

//     std::cout << "\nCargando IoT...\n";
//     cargarDatasetGeneral(red_iot, "..\\datasets\\train_test_network.csv", true, parser_train_test);

//     Inspeccionamos algunos nodos para verificar que todo esté bien construido
//     inspeccionarNodo(red_iot, std::string("2405:6e00:10ce:2c00:9064:17:85d2:57d4"), 20);

//     return 0;
// }