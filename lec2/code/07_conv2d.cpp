// Раздел 8: свёртка изображения 4096x4096 ядром 3x3 — скалярно, авто-векторизация, AVX2.
#include <immintrin.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>
#include "bench.h"

// Скалярная реализация 2D-свёртки (векторизация запрещена, чтобы увидеть чистый скаляр)
__attribute__((noinline, optimize("no-tree-vectorize")))
void conv2d_scalar(const float* I, const float* K, float* O, int H, int W, int kh, int kw) {
    const int out_H = H - kh + 1;
    const int out_W = W - kw + 1;
    for (int i = 0; i < out_H; ++i) {
        for (int j = 0; j < out_W; ++j) {
            float sum = 0.0f;
            for (int m = 0; m < kh; ++m) {
                for (int n = 0; n < kw; ++n) {
                    sum += I[(i + m) * W + (j + n)] * K[m * kw + n];
                }
            }
            O[i * out_W + j] = sum;
        }
    }
}

// Авто-векторизованная свёртка с реструктуризацией циклов
__attribute__((noinline))
void conv2d_autovec(const float* __restrict__ I, const float* __restrict__ K, float* __restrict__ O,
                    int H, int W, int kh, int kw) {
    const int out_H = H - kh + 1;
    const int out_W = W - kw + 1;
    std::fill(O, O + out_H * out_W, 0.0f);
    for (int m = 0; m < kh; ++m) {
        for (int n = 0; n < kw; ++n) {
            const float k_val = K[m * kw + n];
            for (int i = 0; i < out_H; ++i) {
#pragma GCC ivdep
                for (int j = 0; j < out_W; ++j) {
                    O[i * out_W + j] += I[(i + m) * W + (j + n)] * k_val;
                }
            }
        }
    }
}

// Тот же векторизуемый внутренний цикл, но цикл по строкам вынесен наружу:
// строка выхода (16 КБ) остаётся в L1, пока в неё накапливаются все kh*kw слагаемых
__attribute__((noinline))
void conv2d_autovec_rows(const float* __restrict__ I, const float* __restrict__ K, float* __restrict__ O,
                         int H, int W, int kh, int kw) {
    const int out_H = H - kh + 1;
    const int out_W = W - kw + 1;
    for (int i = 0; i < out_H; ++i) {
        float* __restrict__ o = O + size_t(i) * out_W;
        std::fill(o, o + out_W, 0.0f);
        for (int m = 0; m < kh; ++m) {
            const float* in = I + size_t(i + m) * W;
            for (int n = 0; n < kw; ++n) {
                const float k_val = K[m * kw + n];
                for (int j = 0; j < out_W; ++j) o[j] += in[j + n] * k_val;
            }
        }
    }
}

// Ручная AVX2-векторизация 2D-свёртки для ядра 3x3
__attribute__((noinline))
void conv2d_3x3_avx2(const float* __restrict__ I, const float* __restrict__ K, float* __restrict__ O,
                     int H, int W) {
    constexpr int kh = 3, kw = 3;
    const int out_H = H - kh + 1;
    const int out_W = W - kw + 1;

    __m256 k[3][3];
    for (int m = 0; m < 3; ++m)
        for (int n = 0; n < 3; ++n) k[m][n] = _mm256_set1_ps(K[m * kw + n]);

    for (int i = 0; i < out_H; ++i) {
        int j = 0;
        for (; j + 8 <= out_W; j += 8) {
            __m256 sum = _mm256_setzero_ps();
            for (int m = 0; m < 3; ++m) {
                const float* row = &I[(i + m) * W + j];
                for (int n = 0; n < 3; ++n) {
                    __m256 val = _mm256_loadu_ps(&row[n]);
                    sum = _mm256_fmadd_ps(val, k[m][n], sum);
                }
            }
            _mm256_storeu_ps(&O[i * out_W + j], sum);
        }
        for (; j < out_W; ++j) {
            float s = 0.0f;
            for (int m = 0; m < 3; ++m)
                for (int n = 0; n < 3; ++n) s += I[(i + m) * W + j + n] * K[m * 3 + n];
            O[i * out_W + j] = s;
        }
    }
}

int main() {
    const int H = 4096, W = 4096, kh = 3, kw = 3;
    const int oH = H - kh + 1, oW = W - kw + 1;
    std::vector<float> I(size_t(H) * W), O(size_t(oH) * oW), ref(size_t(oH) * oW);
    for (size_t i = 0; i < I.size(); ++i) I[i] = float((i * 7) % 256) / 255.0f;
    std::vector<float> K = {1, 2, 1, 2, 4, 2, 1, 2, 1};  // размытие по Гауссу
    for (float& x : K) x /= 16.0f;

    const double flop = 2.0 * kh * kw * oH * oW;
    auto check = [&] {
        double e = 0;
        for (size_t i = 0; i < O.size(); ++i) e = std::max(e, double(std::fabs(O[i] - ref[i])));
        return e;
    };

    double ts = median_ms([&] { conv2d_scalar(I.data(), K.data(), ref.data(), H, W, kh, kw); }, 1, 5);
    double ta = median_ms([&] { conv2d_autovec(I.data(), K.data(), O.data(), H, W, kh, kw); }, 1, 5);
    double ea = check();
    double tr = median_ms([&] { conv2d_autovec_rows(I.data(), K.data(), O.data(), H, W, kh, kw); }, 1, 5);
    double er = check();
    double tv = median_ms([&] { conv2d_3x3_avx2(I.data(), K.data(), O.data(), H, W); }, 1, 5);
    double ev = check();

    std::printf("Свёртка %dx%d, ядро %dx%d, FP32\n", H, W, kh, kw);
    std::printf("Реализация          Время, мс  GFLOPS  Ускорение  Макс. ошибка\n");
    std::printf("скалярная           %8.1f  %6.2f  %8.2fx  —\n", ts, flop / ts / 1e6, 1.0);
    std::printf("авто-вект. (лекция) %8.1f  %6.2f  %8.2fx  %.1e\n", ta, flop / ta / 1e6, ts / ta, ea);
    std::printf("авто-вект. по строкам%7.1f  %6.2f  %8.2fx  %.1e\n", tr, flop / tr / 1e6, ts / tr, er);
    std::printf("intrinsics AVX2     %8.1f  %6.2f  %8.2fx  %.1e\n", tv, flop / tv / 1e6, ts / tv, ev);
}
