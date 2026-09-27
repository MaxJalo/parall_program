// Раздел 6.3: std::experimental::simd (Parallelism TS 2).
// В лекции используется stdx::assume_aligned<width>(ptr) — такой функции в TS нет.
// Выравнивание там передаётся флагом конструктора: stdx::element_aligned или stdx::vector_aligned.
#include <experimental/simd>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "bench.h"

namespace stdx = std::experimental;

__attribute__((noinline))
void dot_product_simd(const float* a, const float* b, float& result, size_t n) {
    using simd_type = stdx::native_simd<float>;
    constexpr std::size_t width = simd_type::size();

    simd_type vsum(0.0f);

    const size_t n_main = n - n % width;
    size_t i = 0;
    for (; i < n_main; i += width) {
        simd_type va(&a[i], stdx::vector_aligned);
        simd_type vb(&b[i], stdx::vector_aligned);
        vsum += va * vb;
    }

    // Горизонтальная редукция (векторно-оптимизированная)
    result = stdx::reduce(vsum);

    // Обработка хвоста
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
}

__attribute__((noinline, optimize("no-tree-vectorize")))
float dot_scalar(const float* a, const float* b, size_t n) {
    float s = 0.0f;
    for (size_t i = 0; i < n; ++i) s += a[i] * b[i];
    return s;
}

// Условные операции с where
__attribute__((noinline))
void threshold_simd(float* data, float threshold, size_t n) {
    using simd_type = stdx::native_simd<float>;
    constexpr std::size_t width = simd_type::size();

    for (size_t i = 0; i + width <= n; i += width) {
        simd_type v(&data[i], stdx::vector_aligned);
        // where(маска, v) = 0 — обнулить элементы, где маска истинна
        stdx::where(v <= threshold, v) = 0.0f;
        v.copy_to(&data[i], stdx::vector_aligned);
    }
}

int main() {
    using simd_type = stdx::native_simd<float>;
    std::printf("native_simd<float>::size() = %zu (ширина регистра %zu бит)\n\n",
                simd_type::size(), simd_type::size() * 32);

    const size_t n = 4096;
    float* a = static_cast<float*>(std::aligned_alloc(64, n * sizeof(float)));
    float* b = static_cast<float*>(std::aligned_alloc(64, n * sizeof(float)));
    for (size_t i = 0; i < n; ++i) { a[i] = 1.0f; b[i] = (i % 2) ? 0.5f : 1.5f; }

    float r = 0;
    dot_product_simd(a, b, r, n);
    std::printf("dot: simd = %.1f, скалярно = %.1f (ожидается %zu)\n", r, dot_scalar(a, b, n), n);

    std::vector<float> d = {-2, 5, 0.3f, 7, -1, 4, 0.9f, 2};
    float* dd = static_cast<float*>(std::aligned_alloc(64, 64));
    for (size_t i = 0; i < 8; ++i) dd[i] = d[i];
    threshold_simd(dd, 1.0f, 8);
    std::printf("threshold(1.0): ");
    for (size_t i = 0; i < 8; ++i) std::printf("%g ", dd[i]);
    std::printf("\n\n");

    const int reps = 20000;
    double ts = median_ms([&] { float s = dot_scalar(a, b, n); keep(s); }, reps);
    double tv = median_ms([&] { float s; dot_product_simd(a, b, s, n); keep(s); }, reps);
    std::printf("dot скалярный: %.3f нс/элемент\n", ts * 1e6 / n);
    std::printf("dot std::simd: %.3f нс/элемент (%.1fx)\n", tv * 1e6 / n, ts / tv);
    std::free(a); std::free(b); std::free(dd);
}
