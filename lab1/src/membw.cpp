// Задача 1: замер пропускной способности памяти копированием массива (C++).
// Отличия от методички: steady_clock вместо high_resolution_clock (в libstdc++ это system_clock,
// его может подвести NTP), прогревочное копирование до замера, размер задаётся аргументом.
// Сборка: g++ -std=c++17 -O3 -march=native membw.cpp -o membw
// Запуск: ./membw [rows]   (по умолчанию 8000 -> 768 МБ)
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>

void measure_memory_bandwidth_cpp(size_t rows) {
    const size_t cols = rows;
    const size_t channels = 3;
    const size_t total_elements = rows * cols * channels;
    const size_t size_bytes = total_elements * sizeof(float);

    std::cout << "Выделение памяти (" << size_bytes / (1024.0 * 1024.0) << " МБ)...\n";
    std::vector<float> src(total_elements, 1.5f);
    std::vector<float> dst(total_elements, 0.0f);
    std::copy(src.begin(), src.end(), dst.begin());  // прогрев

    const int iterations = rows >= 2000 ? 10 : 1000;
    std::cout << "Выполнение " << iterations << " итераций копирования...\n";

    auto start_time = std::chrono::steady_clock::now();

    for (int iter = 0; iter < iterations; ++iter) {
        // std::copy для тривиальных типов превращается в memmove
        std::copy(src.begin(), src.end(), dst.begin());
    }

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // Объём переданных данных (чтение + запись)
    double total_bytes = iterations * size_bytes * 2.0;
    double bandwidth_gib = total_bytes / (1024.0 * 1024.0 * 1024.0) / elapsed.count();

    std::cout << "Общее время: " << elapsed.count() << " сек\n";
    std::cout << "Фактическая пропускная способность памяти: " << bandwidth_gib << " ГиБ/с ("
              << total_bytes / 1e9 / elapsed.count() << " ГБ/с)\n";
    std::cout << "Контроль: dst[last] = " << dst.back() << "\n";
}

int main(int argc, char** argv) {
    measure_memory_bandwidth_cpp(argc > 1 ? std::strtoul(argv[1], nullptr, 10) : 8000);
    return 0;
}
