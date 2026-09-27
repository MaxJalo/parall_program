// Раздел 2.2–2.3: сколько элементов влезает в регистр (k = W / w) и закон Амдала для SIMD.
#include <immintrin.h>
#include <cstdio>
#include <initializer_list>

double amdahl_simd(double p, int k) { return 1.0 / ((1.0 - p) + p / k); }

int main() {
    std::printf("Регистр  W, бит  FP32  FP64  INT8\n");
    std::printf("__m128   %6zu  %4zu  %4zu  %4zu\n", sizeof(__m128) * 8, sizeof(__m128) / 4,
                sizeof(__m128d) / 8, sizeof(__m128i));
    std::printf("__m256   %6zu  %4zu  %4zu  %4zu\n", sizeof(__m256) * 8, sizeof(__m256) / 4,
                sizeof(__m256d) / 8, sizeof(__m256i));

    std::printf("\nАмдал для SIMD, k = 8 (AVX2, FP32)\n");
    std::printf("   p     S     доля пика\n");
    for (double p : {0.50, 0.80, 0.90, 0.95, 0.964, 0.99, 1.00}) {
        double s = amdahl_simd(p, 8);
        std::printf("%5.3f  %5.2f  %6.1f %%\n", p, s, 100.0 * s / 8);
    }
    // 80 % пика: 1 / ((1-p) + p/8) = 6.4  =>  p = (1 - 1/6.4) / (1 - 1/8)
    std::printf("\nДля 80 %% пика нужно p >= %.3f\n", (1.0 - 1.0 / 6.4) / (1.0 - 1.0 / 8));
}
