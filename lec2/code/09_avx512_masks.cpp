// Раздел 5.5: маскированные операции AVX-512 (из лекции).
// На i5-12450H AVX-512 нет, поэтому файл только компилируется в ассемблер (-mavx512f -S),
// чтобы увидеть регистры масок k0–k7 и инструкции с {k}.
#include <immintrin.h>
#include <cstddef>

void masked_add_avx512(const float* a, const float* b, float* c, const bool* mask, size_t n) {
    for (size_t i = 0; i + 16 <= n; i += 16) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        __mmask16 k = 0;
        for (int j = 0; j < 16; ++j) {
            if (mask[i + j]) k |= (1 << j);
        }
        __m512 vsum = _mm512_add_ps(va, vb);
        _mm512_mask_storeu_ps(&c[i], k, vsum);  // в лекции аргументы перепутаны: (адрес, маска, значение)
    }
}

void clamp_avx512(float* data, float low, float high, size_t n) {
    __m512 vlow = _mm512_set1_ps(low);
    __m512 vhigh = _mm512_set1_ps(high);
    for (size_t i = 0; i + 16 <= n; i += 16) {
        __m512 v = _mm512_loadu_ps(&data[i]);
        v = _mm512_max_ps(v, vlow);
        v = _mm512_min_ps(v, vhigh);
        _mm512_storeu_ps(&data[i], v);
    }
}
