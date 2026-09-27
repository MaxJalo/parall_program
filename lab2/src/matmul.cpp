// Задача 1: умножение матриц C = A * B (N x N, float32) — фрагменты 4–7 методички в одной программе.
// Сборка: g++ -std=c++17 -O3 -march=native -fopenmp-simd src/matmul.cpp -o bin/matmul
// Все версии собираются с одними флагами; «скалярная» отличается только запретом векторизации
// (атрибут), поэтому разница между ней и остальными — ровно вклад SIMD.
// Отличия от методички: steady_clock вместо high_resolution_clock, медиана из нескольких запусков,
// C обнуляется перед каждым запуском (версии 4–6 накапливают в C через +=).
#include <immintrin.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <vector>
#include "bench.h"

// Фрагмент 4. Скалярная версия, порядок циклов i -> k -> j
__attribute__((noinline, optimize("no-tree-vectorize")))
void matmul_scalar(const float* A, const float* B, float* C, int M, int K, int N) {
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            float a = A[i * K + k];
            for (int j = 0; j < N; ++j) {
                C[i * N + j] += a * B[k * N + j];
            }
        }
    }
}

// Фрагмент 5. Авто-векторизация: __restrict + #pragma omp simd
__attribute__((noinline))
void matmul_auto(const float* __restrict A, const float* __restrict B,
                 float* __restrict C, int M, int K, int N) {
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            float a = A[i * K + k];
#pragma omp simd
            for (int j = 0; j < N; ++j) {
                C[i * N + j] += a * B[k * N + j];
            }
        }
    }
}

// Фрагмент 6. Интринсики AVX2 + FMA, скалярный хвост
__attribute__((noinline))
void matmul_avx2(const float* __restrict A, const float* __restrict B,
                 float* __restrict C, int M, int K, int N) {
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            __m256 a = _mm256_set1_ps(A[i * K + k]);
            int j = 0;
            for (; j <= N - 8; j += 8) {
                __m256 b = _mm256_loadu_ps(&B[k * N + j]);
                __m256 c = _mm256_loadu_ps(&C[i * N + j]);
                __m256 res = _mm256_fmadd_ps(a, b, c);
                _mm256_storeu_ps(&C[i * N + j], res);
            }
            for (; j < N; ++j) {
                C[i * N + j] += A[i * K + k] * B[k * N + j];
            }
        }
    }
}

// Фрагмент 7. Блочная версия, блок 64 x 64
constexpr int BLOCK = 64;

__attribute__((noinline))
void matmul_blocked(const float* A, const float* B, float* C, int M, int K, int N) {
    std::memset(C, 0, size_t(M) * N * sizeof(float));
    for (int ii = 0; ii < M; ii += BLOCK) {
        for (int kk = 0; kk < K; kk += BLOCK) {
            for (int jj = 0; jj < N; jj += BLOCK) {
                for (int i = ii; i < std::min(ii + BLOCK, M); ++i) {
                    for (int k = kk; k < std::min(kk + BLOCK, K); ++k) {
                        float a = A[i * K + k];
                        for (int j = jj; j < std::min(jj + BLOCK, N); ++j) {
                            C[i * N + j] += a * B[k * N + j];
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    std::vector<int> sizes;
    for (int a = 1; a < argc; ++a) sizes.push_back(std::atoi(argv[a]));
    if (sizes.empty()) sizes = {256, 512, 1024, 2048};

    std::printf("N,scalar_s,auto_s,avx2_s,blocked_s,scalar_gflops,auto_gflops,avx2_gflops,blocked_gflops,max_err\n");
    for (int n : sizes) {
        const int M = n, K = n, N = n;
        std::vector<float> A(size_t(M) * K), B(size_t(K) * N), C(size_t(M) * N), Cref(size_t(M) * N);
        std::mt19937 gen(42);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        for (auto& x : A) x = dist(gen);
        for (auto& x : B) x = dist(gen);

        const int runs = runs_for(n);
        auto zero = [](std::vector<float>& v) { std::fill(v.begin(), v.end(), 0.0f); };

        double ts = median_s([&] { zero(Cref); matmul_scalar(A.data(), B.data(), Cref.data(), M, K, N); }, runs);
        double ta = median_s([&] { zero(C); matmul_auto(A.data(), B.data(), C.data(), M, K, N); }, runs);
        double err = max_abs_diff(C.data(), Cref.data(), C.size());
        double tv = median_s([&] { zero(C); matmul_avx2(A.data(), B.data(), C.data(), M, K, N); }, runs);
        err = std::max(err, max_abs_diff(C.data(), Cref.data(), C.size()));
        double tb = median_s([&] { matmul_blocked(A.data(), B.data(), C.data(), M, K, N); }, runs);
        err = std::max(err, max_abs_diff(C.data(), Cref.data(), C.size()));

        const double gf = 2.0 * M * N * K / 1e9;
        std::printf("%d,%.6f,%.6f,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%.1e\n", n, ts, ta, tv, tb,
                    gf / ts, gf / ta, gf / tv, gf / tb, err);
        std::fflush(stdout);
    }
}
