#pragma once

#include "medidasCentralidad.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <iomanip>
#include <string>

template <typename VType, typename EType, typename Func>
void medirRendimientoCentralidad(Grafo<VType, EType>& G, Func funcion_centralidad, const std::string& nombre_medida, int repeticiones = 10) {
    std::vector<double> tiempos_ms(repeticiones);

    std::cout << "\nIniciando medicion de rendimiento para: [" << nombre_medida << "] (" << repeticiones << " iteraciones)...\n";

    for (int i = 0; i < repeticiones; i++) {
        auto inicio = std::chrono::high_resolution_clock::now();

        // Ejecutamos la función que pasaste por parámetro
        funcion_centralidad(G);

        auto fin = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duracion = fin - inicio;
        tiempos_ms[i] = duracion.count();
    }

    // ==========================================
    // CÁLCULO DE ESTADÍSTICAS
    // ==========================================
    double suma_tiempos = 0.0;
    for (double t : tiempos_ms) {
        suma_tiempos += t;
    }
    double promedio = suma_tiempos / repeticiones;

    double suma_diferencias_cuadrado = 0.0;
    for (double t : tiempos_ms) {
        double diferencia = t - promedio;
        suma_diferencias_cuadrado += (diferencia * diferencia);
    }
    double varianza = suma_diferencias_cuadrado / (repeticiones - 1); 

    // Imprimir resultados
    std::cout << "--- Resultados de Rendimiento ---\n";
    std::cout << "Medida evaluada  : " << nombre_medida << "\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Tiempo Promedio  : " << promedio << " ms\n";
    std::cout << "Varianza         : " << varianza << " ms^2\n";
}