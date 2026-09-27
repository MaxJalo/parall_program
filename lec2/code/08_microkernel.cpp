// Раздел 9.3: упрощённое SIMD-микро-ядро умножения матриц (AVX2, FP64).
// Меряем, сколько FLOP за такт оно выдаёт, когда A и B лежат в L1.
#include <immintrin.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include "bench.h"

constexpr int MR = 6;
constexpr int NR = 8;

// C[MR x NR] = A[MR x k] * B[k x NR]; B упакован построчно по NR элементов
__attribute__((noinline))
void micro_kernel_avx2(double* C, const double* A, const double* B, int k, int ldc) {
    // Регистры-аккумуляторы: 6 строк × 2 регистра по 4 double = 12 ymm
    __m256d c[MR][2];
    for (int i = 0; i < MR; ++i) {
        c[i][0] = _mm256_setzero_pd();
        c[i][1] = _mm256_setzero_pd();
    }
    for (int p = 0; p < k; ++p) {
        __m256d b0 = _mm256_loadu_pd(&B[p * NR + 0]);
        __m256d b1 = _mm256_loadu_pd(&B[p * NR + 4]);
        for (int i = 0; i < MR; ++i) {
            __m256d a = _mm256_set1_pd(A[i * k + p]);
            c[i][0] = _mm256_fmadd_pd(a, b0, c[i][0]);
            c[i][1] = _mm256_fmadd_pd(a, b1, c[i][1]);
        }
    }
    for (int i = 0; i < MR; ++i) {
        _mm256_storeu_pd(&C[i * ldc + 0], c[i][0]);
        _mm256_storeu_pd(&C[i * ldc + 4], c[i][1]);
    }
}

// Тот же блок наивным тройным циклом
__attribute__((noinline, optimize("no-tree-vectorize")))
void naive_block(double* C, const double* A, const double* B, int k, int ldc) {
    for (int i = 0; i < MR; ++i)
        for (int j = 0; j < NR; ++j) {
            double s = 0;
            for (int p = 0; p < k; ++p) s += A[i * k + p] * B[p * NR + j];
            C[i * ldc + j] = s;
        }
}

int main() {
    const int k = 256;  // A: 6x256 = 12 КБ, B: 256x8 = 16 КБ — вместе в L1 (48 КБ)
    std::vector<double> A(MR * k), B(k * NR), C(MR * NR), Cref(MR * NR);
    for (size_t i = 0; i < A.size(); ++i) A[i] = std::sin(0.01 * i);
    for (size_t i = 0; i < B.size(); ++i) B[i] = std::cos(0.02 * i);

    micro_kernel_avx2(C.data(), A.data(), B.data(), k, NR);
    naive_block(Cref.data(), A.data(), B.data(), k, NR);
    double err = 0;
    for (int i = 0; i < MR * NR; ++i) err = std::fmax(err, std::fabs(C[i] - Cref[i]));

    const double flop = 2.0 * MR * NR * k;
    const int reps = 100000;
    double tk = median_ms([&] { micro_kernel_avx2(C.data(), A.data(), B.data(), k, NR); keep(C[0]); }, reps);
    double tn = median_ms([&] { naive_block(Cref.data(), A.data(), B.data(), k, NR); keep(Cref[0]); }, reps);

    const double peak = 2 * 4 * 2 * 4.4;  // 2 FMA-порта * 4 double * 2 FLOP * 4.4 ГГц
    std::printf("Микро-ядро 6x8, k = %d, FP64; макс. отличие от наивного = %.1e\n", k, err);
    std::printf("Пик FP64 одного P-ядра: %.1f GFLOPS (16 FLOP/такт)\n\n", peak);
    std::printf("Версия         GFLOPS   FLOP/такт @4.4ГГц   доля пика\n");
    for (auto [name, t] : {std::pair{"наивная", tn}, std::pair{"микро-ядро", tk}}) {
        double gf = flop / t / 1e6;
        std::printf("%-14s %7.2f  %8.2f            %5.1f %%\n", name, gf, gf / 4.4, 100 * gf / peak);
    }
}
