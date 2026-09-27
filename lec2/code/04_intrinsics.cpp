// Раздел 5.4: ручная векторизация intrinsics (add_avx2, fma_avx2) против скалярного кода и авто-векторизации.
#include <immintrin.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include "bench.h"

__attribute__((noinline, optimize("no-tree-vectorize")))
void add_scalar(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) c[i] = a[i] + b[i];
}

__attribute__((noinline))
void add_auto(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, size_t n) {
    for (size_t i = 0; i < n; ++i) c[i] = a[i] + b[i];
}

// Ручная векторизация с AVX2: сложение двух массивов
__attribute__((noinline))
void add_avx2(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;

    // Основная часть: обрабатываем по 8 элементов за раз
    constexpr size_t simd_width = 8;  // 256 бит / 32 бита
    size_t n_aligned = n - (n % simd_width);

    for (; i < n_aligned; i += simd_width) {
        __m256 va = _mm256_loadu_ps(&a[i]);  // загрузка 8 float
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);   // 8 сложений за такт
        _mm256_storeu_ps(&c[i], vc);
    }

    // Хвост: скалярная обработка оставшихся элементов
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

// Версия с FMA: c = a * alpha + b. В лекции нет хвоста — добавлен, иначе при n % 8 != 0 конец массива не считается.
__attribute__((noinline))
void fma_avx2(const float* a, const float* b, float* c, float alpha, size_t n) {
    __m256 valpha = _mm256_set1_ps(alpha);  // широковещательная загрузка
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_fmadd_ps(va, valpha, vb);  // c = a*alpha + b
        _mm256_storeu_ps(&c[i], vc);
    }
    for (; i < n; ++i) c[i] = a[i] * alpha + b[i];
}

int main() {
    const size_t n = 4099;  // не кратно 8 — проверяем обработку хвоста
    std::vector<float> a(n), b(n), c(n), ref(n);
    for (size_t i = 0; i < n; ++i) { a[i] = 0.5f * i; b[i] = 1.0f / (i + 1); }

    // Проверка корректности
    add_scalar(a.data(), b.data(), ref.data(), n);
    add_avx2(a.data(), b.data(), c.data(), n);
    double err = 0;
    for (size_t i = 0; i < n; ++i) err = std::fmax(err, std::fabs(c[i] - ref[i]));
    std::printf("add_avx2: макс. отличие от скалярной версии = %g\n", err);
    fma_avx2(a.data(), b.data(), c.data(), 3.0f, n);
    err = 0;
    for (size_t i = 0; i < n; ++i) err = std::fmax(err, std::fabs(c[i] - (a[i] * 3.0f + b[i])));
    std::printf("fma_avx2: макс. отличие = %g (последний элемент %.3f)\n\n", err, c[n - 1]);

    const int reps = 20000;
    auto ns = [&](double ms) { return ms * 1e6 / n; };
    double ts = median_ms([&] { add_scalar(a.data(), b.data(), c.data(), n); keep(c[0]); }, reps);
    double ta = median_ms([&] { add_auto(a.data(), b.data(), c.data(), n); keep(c[0]); }, reps);
    double ti = median_ms([&] { add_avx2(a.data(), b.data(), c.data(), n); keep(c[0]); }, reps);
    double tf = median_ms([&] { fma_avx2(a.data(), b.data(), c.data(), 3.0f, n); keep(c[0]); }, reps);
    std::printf("Версия            нс/элемент  ускорение\n");
    std::printf("скалярная         %.3f       1.00x\n", ns(ts));
    std::printf("авто-векторизация %.3f       %.2fx\n", ns(ta), ts / ta);
    std::printf("add_avx2          %.3f       %.2fx\n", ns(ti), ts / ti);
    std::printf("fma_avx2          %.3f       %.2fx\n", ns(tf), ts / tf);
}
