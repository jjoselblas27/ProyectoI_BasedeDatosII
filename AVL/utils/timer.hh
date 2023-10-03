#pragma once
#include <iostream>
#include <chrono>

template <typename Func>
auto measure_time(Func&& func) {
    auto start_time = std::chrono::high_resolution_clock::now();
    func();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "Tiempo de ejecución: " << duration.count() << " microsegundos" << std::endl;
    
    return duration.count();
}
