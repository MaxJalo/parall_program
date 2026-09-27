// Задача 3: блочная обработка 256x256 и выравнивание данных. Свёртка ядром Гаусса 3x3.
// Фрагменты 16/17 методички (скалярный блочный код, aligned_alloc против std::vector) +
// AVX2-версии, где выравнивание реально влияет на загрузки/записи.
// Запуск:  ./blocking all 4096          — таблица времени всех версий
//          ./blocking one <версия> 2048 — один прогон одной версии (для cachegrind)
#include <immintrin.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <functional>
#include <random>
#include <string>
#include <vector>
#include "bench.h"

constexpr int KH = 3, KW = 3, BS = 256;

// Фрагменты 16 и 17: одинаковый код, отличается только способ выделения памяти в main
__attribute__((noinline))
void convolve2d_blocked(const float* image, const float* kernel, float* result,
                        int H, int W, int kh, int kw, int block_size) {
    int ph = kh / 2, pw = kw / 2;
    for (int i0 = 0; i0 < H; i0 += block_size) {
        for (int j0 = 0; j0 < W; j0 += block_size) {
            int i1 = std::min(i0 + block_size, H);
            int j1 = std::min(j0 + block_size, W);
            for (int i = i0; i < i1; ++i) {
                for (int j = j0; j < j1; ++j) {
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
    }
}

static inline float pixel(const float* img, const float* k, int H, int W, int i, int j) {
    float s = 0.0f;
    for (int ki = 0; ki < KH; ++ki) {
        int ii = i + ki - 1;
        if (ii < 0 || ii >= H) continue;
        for (int kj = 0; kj < KW; ++kj) {
            int jj = j + kj - 1;
            if (jj >= 0 && jj < W) s += img[ii * W + jj] * k[ki * KW + kj];
        }
    }
    return s;
}

// AVX2-свёртка прямоугольника [i0,i1) x [j0,j1). ALIGNED = true: центральная загрузка и запись
// через _mm256_load_ps/_mm256_store_ps (требуют адрес, кратный 32 — иначе segfault).
// Векторная часть идёт по j, кратным 8, начиная с max(j0, 8); края — скалярно.
template <bool ALIGNED>
static inline void conv_rect_avx2(const float* img, const float* k, float* out, int H, int W,
                                  int i0, int i1, int j0, int j1, const __m256 kv[KH][KW]) {
    int jv0 = std::max(j0, 8);
    jv0 = (jv0 + 7) / 8 * 8;
    int jv1 = std::min(j1, W - 8) / 8 * 8;
    if (jv1 < jv0) jv1 = jv0;
    for (int i = i0; i < i1; ++i) {
        for (int j = j0; j < std::min(jv0, j1); ++j) out[i * W + j] = pixel(img, k, H, W, i, j);
        for (int j = jv0; j < jv1; j += 8) {
            __m256 sum = _mm256_setzero_ps();
            for (int ki = 0; ki < KH; ++ki) {
                int ii = i + ki - 1;
                if (ii < 0 || ii >= H) continue;
                const float* row = img + size_t(ii) * W + j;
                __m256 c = ALIGNED ? _mm256_load_ps(row) : _mm256_loadu_ps(row);
                sum = _mm256_fmadd_ps(_mm256_loadu_ps(row - 1), kv[ki][0], sum);
                sum = _mm256_fmadd_ps(c, kv[ki][1], sum);
                sum = _mm256_fmadd_ps(_mm256_loadu_ps(row + 1), kv[ki][2], sum);
            }
            if (ALIGNED) _mm256_store_ps(out + size_t(i) * W + j, sum);
            else _mm256_storeu_ps(out + size_t(i) * W + j, sum);
        }
        for (int j = std::max(jv1, j0); j < j1; ++j) out[i * W + j] = pixel(img, k, H, W, i, j);
    }
}

template <bool ALIGNED>
__attribute__((noinline))
void conv_avx2(const float* img, const float* k, float* out, int H, int W, int block) {
    __m256 kv[KH][KW];
    for (int a = 0; a < KH; ++a)
        for (int b = 0; b < KW; ++b) kv[a][b] = _mm256_set1_ps(k[a * KW + b]);
    if (block <= 0) {
        conv_rect_avx2<ALIGNED>(img, k, out, H, W, 0, H, 0, W, kv);
        return;
    }
    for (int i0 = 0; i0 < H; i0 += block)
        for (int j0 = 0; j0 < W; j0 += block)
            conv_rect_avx2<ALIGNED>(img, k, out, H, W, i0, std::min(i0 + block, H),
                                    j0, std::min(j0 + block, W), kv);
}

struct Buffers {
    // unaligned: смещение на 1 float (4 байта) от выровненного адреса — каждая 2-я загрузка
    // пересекает границу 64-байтной линии
    float *img_al, *out_al, *img_un, *out_un;
    std::vector<float> img_vec, out_vec;
    void* raw[2];
    explicit Buffers(int n) {
        size_t px = size_t(n) * n, bytes = px * sizeof(float);
        img_al = static_cast<float*>(std::aligned_alloc(32, bytes));
        out_al = static_cast<float*>(std::aligned_alloc(32, bytes));
        raw[0] = std::aligned_alloc(64, bytes + 64);
        raw[1] = std::aligned_alloc(64, bytes + 64);
        img_un = static_cast<float*>(raw[0]) + 1;
        out_un = static_cast<float*>(raw[1]) + 1;
        img_vec.resize(px);
        out_vec.resize(px);
        std::mt19937 gen(42);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        for (size_t i = 0; i < px; ++i) img_al[i] = img_un[i] = img_vec[i] = dist(gen);
    }
    ~Buffers() { std::free(img_al); std::free(out_al); std::free(raw[0]); std::free(raw[1]); }
};

int main(int argc, char** argv) {
    std::string mode = argc > 1 ? argv[1] : "all";
    const float kernel[9] = {1.0f / 16, 2.0f / 16, 1.0f / 16, 2.0f / 16, 4.0f / 16,
                             2.0f / 16, 1.0f / 16, 2.0f / 16, 1.0f / 16};

    struct V { const char* name; std::function<void(Buffers&, int)> run; std::function<const float*(Buffers&)> out; };
    std::vector<V> vs = {
        {"frag17_blocked_vector", [&](Buffers& b, int n) { convolve2d_blocked(b.img_vec.data(), kernel, b.out_vec.data(), n, n, KH, KW, BS); },
         [](Buffers& b) { return (const float*)b.out_vec.data(); }},
        {"frag16_blocked_aligned", [&](Buffers& b, int n) { convolve2d_blocked(b.img_al, kernel, b.out_al, n, n, KH, KW, BS); },
         [](Buffers& b) { return (const float*)b.out_al; }},
        {"avx2_unaligned", [&](Buffers& b, int n) { conv_avx2<false>(b.img_un, kernel, b.out_un, n, n, 0); },
         [](Buffers& b) { return (const float*)b.out_un; }},
        {"avx2_aligned", [&](Buffers& b, int n) { conv_avx2<true>(b.img_al, kernel, b.out_al, n, n, 0); },
         [](Buffers& b) { return (const float*)b.out_al; }},
        {"avx2_blocked_unaligned", [&](Buffers& b, int n) { conv_avx2<false>(b.img_un, kernel, b.out_un, n, n, BS); },
         [](Buffers& b) { return (const float*)b.out_un; }},
        {"avx2_blocked_aligned", [&](Buffers& b, int n) { conv_avx2<true>(b.img_al, kernel, b.out_al, n, n, BS); },
         [](Buffers& b) { return (const float*)b.out_al; }},
    };

    if (mode == "one" && argc >= 4) {
        int n = std::atoi(argv[3]);
        Buffers b(n);
        for (auto& v : vs)
            if (v.name == std::string(argv[2])) { v.run(b, n); std::printf("%s done, out[1]=%f\n", v.name, v.out(b)[1]); }
        return 0;
    }

    int n = argc > 2 ? std::atoi(argv[2]) : 4096;
    Buffers b(n);
    std::vector<float> ref(size_t(n) * n);
    convolve2d_blocked(b.img_vec.data(), kernel, ref.data(), n, n, KH, KW, 0x7fffffff);
    const double flop = 2.0 * 9 * double(n) * n;
    std::printf("Свёртка %dx%d, Гаусс 3x3, блок %d; адреса %% 64: aligned=%zu, unaligned=%zu, vector=%zu\n",
                n, n, BS, reinterpret_cast<uintptr_t>(b.img_al) % 64, reinterpret_cast<uintptr_t>(b.img_un) % 64,
                reinterpret_cast<uintptr_t>(b.img_vec.data()) % 64);
    std::printf("%-24s %9s %8s %8s %10s\n", "версия", "мс", "GFLOPS", "% пика", "ошибка");
    for (auto& v : vs) {
        double t = median_s([&] { v.run(b, n); }, 7);
        double err = max_abs_diff(v.out(b), ref.data(), ref.size());
        std::printf("%-24s %9.3f %8.2f %7.1f%% %10.1e\n", v.name, t * 1e3, flop / t / 1e9,
                    100 * flop / t / 1e9 / 140.8, err);
    }
}
