// Раздел 4.4: управление векторизацией прагмами и атрибутами.
#include <cmath>
#include <cstdio>
#include <vector>
#include "bench.h"

// ivdep: программист обещает, что зависимостей между итерациями нет.
// Здесь обещание честное: a и b — разные массивы, но без __restrict__ компилятор этого не знает.
__attribute__((noinline))
void scale_no_hint(float* a, const float* b, size_t n) {
    for (size_t i = 0; i < n; ++i) a[i] = b[i] * 2.0f;
}

__attribute__((noinline))
void scale_ivdep(float* a, const float* b, size_t n) {
#pragma GCC ivdep
    for (size_t i = 0; i < n; ++i) a[i] = b[i] * 2.0f;
}

// Отключение векторизации для одной функции (в GCC — атрибутом, clang-прагма GCC не понимает)
__attribute__((noinline, optimize("no-tree-vectorize")))
void no_vectorize(float* a, const float* b, size_t n) {
    for (size_t i = 0; i < n; ++i) a[i] = b[i] * 2.0f;
}

// SIMD-функция для вызова из циклов
#pragma omp declare simd notinbranch uniform(factor)
float transform(float x, float factor) {
    return std::sin(x) * factor;
}

__attribute__((noinline))
void apply_transform(const float* in, float* out, float factor, size_t n) {
#pragma omp simd
    for (size_t i = 0; i < n; ++i) {
        out[i] = transform(in[i], factor);  // векторизованный вызов
    }
}

__attribute__((noinline, optimize("no-tree-vectorize")))
void apply_transform_scalar(const float* in, float* out, float factor, size_t n) {
    for (size_t i = 0; i < n; ++i) out[i] = std::sin(in[i]) * factor;
}

int main() {
    const size_t n = 4096;
    std::vector<float> a(n), b(n);
    for (size_t i = 0; i < n; ++i) b[i] = 0.001f * i;
    const int reps = 20000;

    auto ns = [&](double ms) { return ms * 1e6 / n; };
    std::printf("Функция                          нс на элемент\n");
    std::printf("scale без подсказок              %.3f\n",
                ns(median_ms([&] { scale_no_hint(a.data(), b.data(), n); keep(a[0]); }, reps)));
    std::printf("scale + #pragma GCC ivdep        %.3f\n",
                ns(median_ms([&] { scale_ivdep(a.data(), b.data(), n); keep(a[0]); }, reps)));
    std::printf("scale, векторизация отключена    %.3f\n",
                ns(median_ms([&] { no_vectorize(a.data(), b.data(), n); keep(a[0]); }, reps)));
    std::printf("sin: omp declare simd            %.3f\n",
                ns(median_ms([&] { apply_transform(b.data(), a.data(), 2.0f, n); keep(a[0]); }, 2000)));
    std::printf("sin: скалярный                   %.3f\n",
                ns(median_ms([&] { apply_transform_scalar(b.data(), a.data(), 2.0f, n); keep(a[0]); }, 2000)));
}
