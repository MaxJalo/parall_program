// Пропускная способность чтения в зависимости от размера рабочего множества.
// Ступеньки на графике соответствуют границам L1d / L2 / L3 / RAM.
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <vector>

static volatile float sink;

// 32 независимых накопителя = 4 ymm-регистра. С одним накопителем цикл упирается
// в задержку vaddps (4 такта), и границы L1/L2 на графике не видны.
__attribute__((noinline))
float read_sum(const float* __restrict__ a, size_t n) {
    float acc[32] = {};
    for (size_t i = 0; i < n; i += 32)
        for (int j = 0; j < 32; ++j) acc[j] += a[i + j];
    float s = 0.0f;
    for (float x : acc) s += x;
    return s;
}

int main() {
    std::printf("size_kb,gbps\n");
    for (size_t bytes = 8 * 1024; bytes <= 512ull * 1024 * 1024; bytes *= 2) {
        const size_t n = bytes / sizeof(float);
        std::vector<float> a(n, 1.0f);

        // каждый размер читаем суммарно ~4 ГБ, чтобы маленькие массивы мерились честно
        const size_t reps = std::max<size_t>(1, (4ull << 30) / bytes);

        double best = 1e30;
        for (int trial = 0; trial < 3; ++trial) {
            auto t0 = std::chrono::steady_clock::now();
            for (size_t r = 0; r < reps; ++r) sink = read_sum(a.data(), n);
            auto t1 = std::chrono::steady_clock::now();
            best = std::min(best, std::chrono::duration<double>(t1 - t0).count());
        }
        std::printf("%zu,%.2f\n", bytes / 1024, double(bytes) * reps / best / 1e9);
    }
    return 0;
}
