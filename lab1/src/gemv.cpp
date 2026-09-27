// Индивидуальное задание 1, вариант 6: GEMV y = A*x, A — 2000 x 4000, float32.
// Сборка: см. run_all.sh (-O0, -O3 -march=native, -O3 -march=native -ffast-math)
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

constexpr size_t M = 2000;  // строк
constexpr size_t N = 4000;  // столбцов
constexpr int RUNS = 30;

// 1. Обход по строкам (row-major): y[i] = скалярное произведение строки i на x
__attribute__((noinline))
void gemv_rows(const float* __restrict__ A, const float* __restrict__ x, float* __restrict__ y) {
    for (size_t i = 0; i < M; ++i) {
        const float* row = A + i * N;
        float sum = 0.0f;
        for (size_t j = 0; j < N; ++j) sum += row[j] * x[j];
        y[i] = sum;
    }
}

// 2. То же, но редукция явно разрешена к векторизации (нужен -fopenmp-simd, -ffast-math не нужен)
__attribute__((noinline))
void gemv_rows_simd(const float* __restrict__ A, const float* __restrict__ x, float* __restrict__ y) {
    for (size_t i = 0; i < M; ++i) {
        const float* row = A + i * N;
        float sum = 0.0f;
#pragma omp simd reduction(+ : sum)
        for (size_t j = 0; j < N; ++j) sum += row[j] * x[j];
        y[i] = sum;
    }
}

// 3. Обход по столбцам той же row-major матрицы: внутренний цикл идёт с шагом N*4 = 16 КБ
__attribute__((noinline))
void gemv_cols(const float* __restrict__ A, const float* __restrict__ x, float* __restrict__ y) {
    std::fill(y, y + M, 0.0f);
    for (size_t j = 0; j < N; ++j) {
        const float xj = x[j];
        for (size_t i = 0; i < M; ++i) y[i] += A[i * N + j] * xj;
    }
}

// 4. Обход по столбцам, но матрица хранится по столбцам (column-major, At[j*M + i] = A[i][j]):
//    y += x[j] * столбец j — это AXPY, редукции нет, доступ последовательный
__attribute__((noinline))
void gemv_axpy_colmajor(const float* __restrict__ At, const float* __restrict__ x, float* __restrict__ y) {
    std::fill(y, y + M, 0.0f);
    for (size_t j = 0; j < N; ++j) {
        const float xj = x[j];
        const float* col = At + j * M;
        for (size_t i = 0; i < M; ++i) y[i] += col[i] * xj;
    }
}

template <typename F>
double median_ms(F&& f) {
    std::vector<double> t;
    f();  // прогрев
    for (int r = 0; r < RUNS; ++r) {
        auto t0 = std::chrono::steady_clock::now();
        f();
        auto t1 = std::chrono::steady_clock::now();
        t.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
    }
    std::sort(t.begin(), t.end());
    return t[t.size() / 2];
}

float* alloc64(size_t count) {
    size_t bytes = (count * sizeof(float) + 63) / 64 * 64;  // aligned_alloc: размер кратен выравниванию
    return static_cast<float*>(std::aligned_alloc(64, bytes));
}

int main() {
    float* A = alloc64(M * N);
    float* At = alloc64(M * N);
    float* x = alloc64(N);
    float* y = alloc64(M);

    std::srand(42);
    for (size_t i = 0; i < M * N; ++i) A[i] = float(std::rand()) / RAND_MAX - 0.5f;
    for (size_t j = 0; j < N; ++j) x[j] = float(std::rand()) / RAND_MAX - 0.5f;
    for (size_t i = 0; i < M; ++i)
        for (size_t j = 0; j < N; ++j) At[j * M + i] = A[i * N + j];

    // Эталон в double
    std::vector<double> ref(M);
    for (size_t i = 0; i < M; ++i) {
        double s = 0;
        for (size_t j = 0; j < N; ++j) s += double(A[i * N + j]) * x[j];
        ref[i] = s;
    }
    auto max_err = [&] {
        double e = 0;
        for (size_t i = 0; i < M; ++i) e = std::max(e, std::fabs(y[i] - ref[i]));
        return e;
    };

    const double flop = 2.0 * M * N;
    const double bytes = 4.0 * (M * N + N + M);
    const double roof = 38.4 * flop / bytes;  // потолок Roofline при 38.4 ГБ/с, GFLOPS

    std::printf("GEMV %zux%zu float32: %.1f MFLOP, %.1f МБ, AI = %.3f FLOP/байт, потолок %.1f GFLOPS\n",
                M, N, flop / 1e6, bytes / 1e6, flop / bytes, roof);
    std::printf("Адреса %% 64: A=%zu At=%zu x=%zu y=%zu; медиана из %d запусков\n\n",
                reinterpret_cast<uintptr_t>(A) % 64, reinterpret_cast<uintptr_t>(At) % 64,
                reinterpret_cast<uintptr_t>(x) % 64, reinterpret_cast<uintptr_t>(y) % 64, RUNS);
    std::printf("%-28s %9s %8s %8s %11s %9s\n", "Вариант", "мс", "GFLOPS", "ГБ/с", "% потолка", "ошибка");

    struct V { const char* name; void (*f)(const float*, const float*, float*); const float* mat; };
    V variants[] = {
        {"1. по строкам", gemv_rows, A},
        {"2. по строкам + omp simd", gemv_rows_simd, A},
        {"3. по столбцам (шаг 16 КБ)", gemv_cols, A},
        {"4. AXPY, column-major", gemv_axpy_colmajor, At},
    };
    for (auto& v : variants) {
        double ms = median_ms([&] { v.f(v.mat, x, y); });
        double gf = flop / ms / 1e6;
        std::printf("%-28s %9.3f %8.2f %8.2f %10.1f%% %9.1e\n", v.name, ms, gf, bytes / ms / 1e6,
                    100 * gf / roof, max_err());
    }
    std::free(A); std::free(At); std::free(x); std::free(y);
}
