// Раздел 4.2: примеры 1–3 из лекции. Собирается дважды — с векторизацией и без —
// и печатает время каждого примера. Отчёт компилятора смотрим флагом -fopt-info-vec-all.
#include <cstdio>
#include <vector>
#include "bench.h"

// Пример 1. Векторизуемый цикл
__attribute__((noinline))
void vectorizable_example(const float* __restrict__ a, const float* __restrict__ b,
                          float* __restrict__ c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

// Пример 2. Невекторизуемый цикл (зависимость по данным)
__attribute__((noinline))
void non_vectorizable_example(float* a, const float* b, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        a[i] = a[i - 1] + b[i];  // рекуррентность
    }
}

// Пример 3. Условно векторизуемый (требует масок)
__attribute__((noinline))
void conditional_vectorizable(const float* a, float* b, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (a[i] > 0.0f) {
            b[i] = a[i] * a[i];
        }
    }
}

// 3б. То же, но с __restrict__: компилятор знает, что a и b не пересекаются
__attribute__((noinline))
void conditional_restrict(const float* __restrict__ a, float* __restrict__ b, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (a[i] > 0.0f) {
            b[i] = a[i] * a[i];
        }
    }
}

// 3в. Запись без условия: маска превращается в blend (выбор из двух значений)
__attribute__((noinline))
void conditional_select(const float* __restrict__ a, float* __restrict__ b, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        b[i] = (a[i] > 0.0f) ? a[i] * a[i] : b[i];
    }
}

int main() {
    const size_t n = 4096;  // 3 массива по 16 КБ — всё в L1, меряем вычисления, а не память
    std::vector<float> a(n), b(n), c(n);
    for (size_t i = 0; i < n; ++i) {
        a[i] = (i % 3 == 0) ? -1.0f : 0.5f;
        b[i] = 1e-6f * i;
    }
    const int reps = 20000;

    double t1 = median_ms([&] { vectorizable_example(a.data(), b.data(), c.data(), n); keep(c[0]); }, reps);
    double t2 = median_ms([&] { non_vectorizable_example(c.data(), b.data(), n); keep(c[0]); }, reps);
    double t3 = median_ms([&] { conditional_vectorizable(a.data(), c.data(), n); keep(c[0]); }, reps);

    double t3b = median_ms([&] { conditional_restrict(a.data(), c.data(), n); keep(c[0]); }, reps);
    double t3c = median_ms([&] { conditional_select(a.data(), c.data(), n); keep(c[0]); }, reps);

    std::printf("Пример                          нс на элемент\n");
    std::printf("1.  c[i] = a[i] + b[i]          %.3f\n", t1 * 1e6 / n);
    std::printf("2.  a[i] = a[i-1] + b[i]        %.3f\n", t2 * 1e6 / n);
    std::printf("3.  if (a[i] > 0) b[i] = a*a    %.3f\n", t3 * 1e6 / n);
    std::printf("3б. то же + __restrict__        %.3f\n", t3b * 1e6 / n);
    std::printf("3в. b[i] = a>0 ? a*a : b[i]     %.3f\n", t3c * 1e6 / n);
}
