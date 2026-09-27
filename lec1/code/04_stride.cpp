// Фрагмент 2: Анализ производительности при различных шагах доступа к памяти
// Отличие от лекции: steady_clock вместо high_resolution_clock
// (в libstdc++ high_resolution_clock — это system_clock, не монотонные часы).
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

long long traverse_with_stride(const std::vector<int>& data, int stride) {
    long long sum = 0;
    for (size_t i = 0; i < data.size(); i += stride) {
        sum += data[i];
    }
    return sum;
}

int main() {
    const size_t N = 100'000'000;
    std::vector<int> data(N);
    std::iota(data.begin(), data.end(), 1);

    const std::vector<int> strides = {1, 2, 4, 8, 16, 32, 64, 128, 256};

    std::cout << std::left << std::setw(8) << "Шаг"
              << std::setw(14) << "Время (мс)"
              << std::setw(16) << "Полезные ГБ/с"
              << std::setw(16) << "нс/обращение" << "\n";
    std::cout << std::string(54, '-') << "\n";

    for (int stride : strides) {
        volatile long long warmup = traverse_with_stride(data, stride);
        (void)warmup;

        auto start = std::chrono::steady_clock::now();
        volatile long long result = traverse_with_stride(data, stride);
        (void)result;
        auto end = std::chrono::steady_clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        double accesses = static_cast<double>(N / stride);
        double bytes = accesses * sizeof(int);
        std::cout << std::left << std::setw(8) << stride
                  << std::setw(14) << std::fixed << std::setprecision(2) << ms
                  << std::setw(16) << (bytes / 1e9) / (ms / 1e3)
                  << std::setw(16) << ms * 1e6 / accesses << "\n";
    }
    return 0;
}
