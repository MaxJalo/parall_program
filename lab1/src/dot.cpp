// Задача 2: скалярное произведение, N = 10^7 float. Собирается с разными флагами (run_flags.sh).
// Отличия от методички:
//   * 10_000_000 — не C++ (это синтаксис Python); в C++14+ разделитель цифр — апостроф: 10'000'000;
//   * steady_clock вместо high_resolution_clock;
//   * один запуск длится ~5 мс и сильно шумит, поэтому берём медиану из 21 запуска.
#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

// Скалярное произведение
__attribute__((noinline))
float dot_product(const float* a, const float* b, size_t n) {
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

int main() {
    const size_t n = 10'000'000;
    std::vector<float> a(n, 1.5f);
    std::vector<float> b(n, 2.5f);

    std::cout << "Размерность векторов: " << n << "\n";

    float result = 0.0f;
    std::vector<double> times;
    for (int run = 0; run < 21; ++run) {
        auto start = std::chrono::steady_clock::now();
        result = dot_product(a.data(), b.data(), n);
        auto end = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }
    std::sort(times.begin(), times.end());

    std::cout << "Результат скалярного произведения: " << result
              << " (точно: " << 1.5 * 2.5 * n << ")\n";
    std::cout << "Время выполнения (медиана из 21): " << times[times.size() / 2] << " мс"
              << " (min " << times.front() << ", max " << times.back() << ")\n";
    return 0;
}
