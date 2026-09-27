// Задача 2 и индивидуальное задание (вариант 6): двумерная свёртка (кросс-корреляция), режим 'same',
// нули за границей. Фрагменты 11–14 методички + две мои версии без ветвлений во внутреннем цикле.
// Запуск: ./conv gauss3 512 1024 2048 4096   или   ./conv box7 512 1024 2048 4096
// Сборка: g++ -std=c++17 -O3 -march=native src/conv.cpp -o bin/conv
#include <immintrin.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <vector>
#include "bench.h"

// Фрагмент 11. Скалярная свёртка (векторизация запрещена атрибутом)
__attribute__((noinline, optimize("no-tree-vectorize")))
void convolve2d_scalar(const float* image, const float* kernel, float* result,
                       int H, int W, int kh, int kw) {
    int ph = kh / 2, pw = kw / 2;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            float s = 0.0f;
            for (int ki = 0; ki < kh; ++ki) {
                for (int kj = 0; kj < kw; ++kj) {
                    int ii = i + ki - ph;
                    int jj = j + kj - pw;
                    if (ii >= 0 && ii < H && jj >= 0 && jj < W) {
                        s += image[ii * W + jj] * kernel[ki * kw + kj];
                    }
                }
            }
            result[i * W + j] = s;
        }
    }
}

// Фрагмент 12. «Авто-векторизация» — тот же код с __restrict
__attribute__((noinline))
void convolve2d_auto(const float* __restrict image, const float* __restrict kernel,
                     float* __restrict result, int H, int W, int kh, int kw) {
    int ph = kh / 2, pw = kw / 2;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            float s = 0.0f;
            for (int ki = 0; ki < kh; ++ki) {
                for (int kj = 0; kj < kw; ++kj) {
                    int ii = i + ki - ph;
                    int jj = j + kj - pw;
                    if (ii >= 0 && ii < H && jj >= 0 && jj < W) {
                        s += image[ii * W + jj] * kernel[ki * kw + kj];
                    }
                }
            }
            result[i * W + j] = s;
        }
    }
}

// Скалярный расчёт одного пикселя с проверкой границ (для краёв в векторных версиях)
static inline float pixel_bounded(const float* image, const float* kernel, int H, int W,
                                  int kh, int kw, int i, int j) {
    int ph = kh / 2, pw = kw / 2;
    float s = 0.0f;
    for (int ki = 0; ki < kh; ++ki) {
        int ii = i + ki - ph;
        if (ii < 0 || ii >= H) continue;
        for (int kj = 0; kj < kw; ++kj) {
            int jj = j + kj - pw;
            if (jj >= 0 && jj < W) s += image[ii * W + jj] * kernel[ki * kw + kj];
        }
    }
    return s;
}

// Моя версия: проверки границ вынесены из внутреннего цикла. Для каждой строки выхода
// накапливаем kh*kw сдвинутых строк входа; внутренний цикл по j — непрерывный и без if.
__attribute__((noinline))
void convolve2d_auto_nobranch(const float* __restrict image, const float* __restrict kernel,
                              float* __restrict result, int H, int W, int kh, int kw) {
    int ph = kh / 2, pw = kw / 2;
    for (int i = 0; i < H; ++i) {
        float* __restrict out = result + size_t(i) * W;
        std::fill(out, out + W, 0.0f);
        for (int ki = 0; ki < kh; ++ki) {
            int ii = i + ki - ph;
            if (ii < 0 || ii >= H) continue;
            const float* in = image + size_t(ii) * W;
            for (int kj = 0; kj < kw; ++kj) {
                const float kv = kernel[ki * kw + kj];
                const int off = kj - pw;
                const int j0 = std::max(0, -off), j1 = std::min(W, W - off);
                for (int j = j0; j < j1; ++j) out[j] += in[j + off] * kv;
            }
        }
    }
}

// Фрагмент 13. Интринсики AVX2: левая граница скалярно, середина по 8, правая граница и хвост скалярно
__attribute__((noinline))
void convolve2d_avx2(const float* image, const float* kernel, float* result,
                     int H, int W, int kh, int kw) {
    int ph = kh / 2, pw = kw / 2;
    for (int i = 0; i < H; ++i) {
        int j_start = std::max(0, pw);
        int j_end = std::min(W, W - pw);
        for (int j = 0; j < j_start; ++j)
            result[i * W + j] = pixel_bounded(image, kernel, H, W, kh, kw, i, j);
        int j = j_start;
        for (; j <= j_end - 8; j += 8) {
            __m256 sum = _mm256_setzero_ps();
            for (int ki = 0; ki < kh; ++ki) {
                int ii = i + ki - ph;
                if (ii < 0 || ii >= H) continue;
                for (int kj = 0; kj < kw; ++kj) {
                    __m256 kvec = _mm256_set1_ps(kernel[ki * kw + kj]);
                    __m256 img_vec = _mm256_loadu_ps(&image[ii * W + j + kj - pw]);
                    sum = _mm256_fmadd_ps(kvec, img_vec, sum);
                }
            }
            _mm256_storeu_ps(&result[i * W + j], sum);
        }
        for (; j < W; ++j)
            result[i * W + j] = pixel_bounded(image, kernel, H, W, kh, kw, i, j);
    }
}

// Фрагмент 14. Разделяемое ядро: горизонтальный проход, затем вертикальный
__attribute__((noinline))
void convolve1d_horizontal(const float* src, float* dst, int H, int W, const float* kernel, int kw) {
    int pw = kw / 2;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            float s = 0.0f;
            for (int kj = 0; kj < kw; ++kj) {
                int jj = j + kj - pw;
                if (jj >= 0 && jj < W) s += src[i * W + jj] * kernel[kj];
            }
            dst[i * W + j] = s;
        }
    }
}

__attribute__((noinline))
void convolve1d_vertical(const float* src, float* dst, int H, int W, const float* kernel, int kh) {
    int ph = kh / 2;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            float s = 0.0f;
            for (int ki = 0; ki < kh; ++ki) {
                int ii = i + ki - ph;
                if (ii >= 0 && ii < H) s += src[ii * W + j] * kernel[ki];
            }
            dst[i * W + j] = s;
        }
    }
}

void convolve2d_separable(const float* image, float* result, float* temp, int H, int W,
                          const float* kx, const float* ky, int kh, int kw) {
    convolve1d_horizontal(image, temp, H, W, kx, kw);
    convolve1d_vertical(temp, result, H, W, ky, kh);
}

// Моя версия разделяемой свёртки: оба прохода с непрерывным внутренним циклом по j без if
__attribute__((noinline))
void convolve2d_separable_vec(const float* __restrict image, float* __restrict result,
                              float* __restrict temp, int H, int W,
                              const float* kx, const float* ky, int kh, int kw) {
    int ph = kh / 2, pw = kw / 2;
    for (int i = 0; i < H; ++i) {
        const float* __restrict in = image + size_t(i) * W;
        float* __restrict t = temp + size_t(i) * W;
        std::fill(t, t + W, 0.0f);
        for (int kj = 0; kj < kw; ++kj) {
            const float kv = kx[kj];
            const int off = kj - pw;
            const int j0 = std::max(0, -off), j1 = std::min(W, W - off);
            for (int j = j0; j < j1; ++j) t[j] += in[j + off] * kv;
        }
    }
    for (int i = 0; i < H; ++i) {
        float* __restrict out = result + size_t(i) * W;
        std::fill(out, out + W, 0.0f);
        for (int ki = 0; ki < kh; ++ki) {
            int ii = i + ki - ph;
            if (ii < 0 || ii >= H) continue;
            const float kv = ky[ki];
            const float* __restrict src = temp + size_t(ii) * W;
            for (int j = 0; j < W; ++j) out[j] += src[j] * kv;
        }
    }
}

int main(int argc, char** argv) {
    std::string kname = argc > 1 ? argv[1] : "gauss3";
    std::vector<int> sizes;
    for (int a = 2; a < argc; ++a) sizes.push_back(std::atoi(argv[a]));
    if (sizes.empty()) sizes = {512, 1024, 2048, 4096};

    int ksz;
    std::vector<float> k1d;
    if (kname == "box7") {
        ksz = 7;
        k1d.assign(7, 1.0f / 7.0f);
    } else {
        ksz = 3;
        k1d = {0.25f, 0.5f, 0.25f};
    }
    std::vector<float> kernel(ksz * ksz);
    for (int a = 0; a < ksz; ++a)
        for (int b = 0; b < ksz; ++b) kernel[a * ksz + b] = k1d[a] * k1d[b];

    std::printf("N,scalar_s,auto_s,auto_nobranch_s,avx2_s,separable_s,separable_vec_s,max_err\n");
    for (int n : sizes) {
        const int H = n, W = n;
        const size_t px = size_t(H) * W;
        std::vector<float> image(px), ref(px), out(px), temp(px);
        std::mt19937 gen(42);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        for (auto& x : image) x = dist(gen);

        const int runs = runs_for(n);
        double t_s = median_s([&] { convolve2d_scalar(image.data(), kernel.data(), ref.data(), H, W, ksz, ksz); }, runs);
        double t_a = median_s([&] { convolve2d_auto(image.data(), kernel.data(), out.data(), H, W, ksz, ksz); }, runs);
        double err = max_abs_diff(out.data(), ref.data(), px);
        double t_n = median_s([&] { convolve2d_auto_nobranch(image.data(), kernel.data(), out.data(), H, W, ksz, ksz); }, runs);
        err = std::max(err, max_abs_diff(out.data(), ref.data(), px));
        double t_v = median_s([&] { convolve2d_avx2(image.data(), kernel.data(), out.data(), H, W, ksz, ksz); }, runs);
        err = std::max(err, max_abs_diff(out.data(), ref.data(), px));
        double t_p = median_s([&] { convolve2d_separable(image.data(), out.data(), temp.data(), H, W, k1d.data(), k1d.data(), ksz, ksz); }, runs);
        err = std::max(err, max_abs_diff(out.data(), ref.data(), px));
        double t_pv = median_s([&] { convolve2d_separable_vec(image.data(), out.data(), temp.data(), H, W, k1d.data(), k1d.data(), ksz, ksz); }, runs);
        err = std::max(err, max_abs_diff(out.data(), ref.data(), px));

        std::printf("%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.1e\n", n, t_s, t_a, t_n, t_v, t_p, t_pv, err);
        std::fflush(stdout);
    }
}
