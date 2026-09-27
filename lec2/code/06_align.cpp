// Раздел 7: способы выравнивания и цена невыровненной загрузки.
#include <immintrin.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>
#include <new>
#include <vector>
#include "bench.h"

// Кастомный аллокатор с выравниванием для STL-контейнеров (раздел 7.4)
template <typename T, std::size_t Alignment = 64>
struct AlignedAllocator {
    using value_type = T;
    AlignedAllocator() noexcept = default;
    template <typename U>
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}
    template <typename U>
    struct rebind { using other = AlignedAllocator<U, Alignment>; };

    T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) throw std::bad_array_new_length();
        // aligned_alloc требует, чтобы размер был кратен выравниванию
        std::size_t bytes = (n * sizeof(T) + Alignment - 1) / Alignment * Alignment;
        void* ptr = std::aligned_alloc(Alignment, bytes);
        if (!ptr) throw std::bad_alloc();
        return static_cast<T*>(ptr);
    }
    void deallocate(T* ptr, std::size_t) noexcept { std::free(ptr); }
};
template <typename T, typename U, std::size_t A>
bool operator==(const AlignedAllocator<T, A>&, const AlignedAllocator<U, A>&) { return true; }

template <typename T>
using AlignedVector = std::vector<T, AlignedAllocator<T, 64>>;

struct alignas(64) AlignedBlock { float data[1024]; };

// Сумма массива выровненными загрузками (_mm256_load_ps требует адрес, кратный 32)
__attribute__((noinline))
float sum_load(const float* p, size_t n) {
    __m256 s0 = _mm256_setzero_ps(), s1 = s0, s2 = s0, s3 = s0;
    for (size_t i = 0; i < n; i += 32) {
        s0 = _mm256_add_ps(s0, _mm256_load_ps(p + i));
        s1 = _mm256_add_ps(s1, _mm256_load_ps(p + i + 8));
        s2 = _mm256_add_ps(s2, _mm256_load_ps(p + i + 16));
        s3 = _mm256_add_ps(s3, _mm256_load_ps(p + i + 24));
    }
    __m256 s = _mm256_add_ps(_mm256_add_ps(s0, s1), _mm256_add_ps(s2, s3));
    float out[8];
    _mm256_storeu_ps(out, s);
    return out[0] + out[1] + out[2] + out[3] + out[4] + out[5] + out[6] + out[7];
}

// То же с невыровненными загрузками — работает с любым адресом
__attribute__((noinline))
float sum_loadu(const float* p, size_t n) {
    __m256 s0 = _mm256_setzero_ps(), s1 = s0, s2 = s0, s3 = s0;
    for (size_t i = 0; i < n; i += 32) {
        s0 = _mm256_add_ps(s0, _mm256_loadu_ps(p + i));
        s1 = _mm256_add_ps(s1, _mm256_loadu_ps(p + i + 8));
        s2 = _mm256_add_ps(s2, _mm256_loadu_ps(p + i + 16));
        s3 = _mm256_add_ps(s3, _mm256_loadu_ps(p + i + 24));
    }
    __m256 s = _mm256_add_ps(_mm256_add_ps(s0, s1), _mm256_add_ps(s2, s3));
    float out[8];
    _mm256_storeu_ps(out, s);
    return out[0] + out[1] + out[2] + out[3] + out[4] + out[5] + out[6] + out[7];
}

int main() {
    auto mod64 = [](const void* p) { return reinterpret_cast<uintptr_t>(p) % 64; };

    float* p1 = static_cast<float*>(std::aligned_alloc(32, 1024 * sizeof(float)));
    AlignedBlock blk;
    void* p3 = nullptr;
    if (posix_memalign(&p3, 64, 1024 * sizeof(float)) != 0) return 1;
    float* p4 = static_cast<float*>(_mm_malloc(1024 * sizeof(float), 64));
    AlignedVector<float> v(1024, 1.0f);
    std::vector<float> plain(1024);
    float* p6 = static_cast<float*>(std::malloc(1024 * sizeof(float) + 4)) ;

    std::printf("Способ                          адрес %% 64\n");
    std::printf("std::aligned_alloc(32, ...)     %zu\n", mod64(p1));
    std::printf("struct alignas(64)              %zu\n", mod64(blk.data));
    std::printf("posix_memalign(64)              %zu\n", mod64(p3));
    std::printf("_mm_malloc(64)                  %zu\n", mod64(p4));
    std::printf("vector + AlignedAllocator<64>   %zu\n", mod64(v.data()));
    std::printf("обычный std::vector             %zu\n", mod64(plain.data()));
    std::printf("malloc + 4 байта                %zu\n\n", mod64(p6 + 1));
    std::free(p1); std::free(p3); _mm_free(p4); std::free(p6);

    // std::assume_aligned (C++20): обещание компилятору, проверки нет
    const float* hinted = std::assume_aligned<64>(v.data());
    std::printf("assume_aligned<64>: сумма = %.0f\n\n", sum_load(hinted, 1024));

    // Замер: один и тот же буфер, смещение 0 байт (выровнено) и 4 байта (каждая 2-я загрузка
    // пересекает границу 64-байтной линии — split load)
    std::printf("Размер     load, ГБ/с  loadu+0, ГБ/с  loadu+4Б, ГБ/с  split-штраф\n");
    for (size_t kb : {16, 256, 4096, 262144}) {
        size_t n = kb * 1024 / sizeof(float);
        float* buf = static_cast<float*>(std::aligned_alloc(64, (n + 64) * sizeof(float)));
        for (size_t i = 0; i < n + 64; ++i) buf[i] = 1.0f;
        int reps = static_cast<int>(std::max<size_t>(1, (1ull << 31) / (n * 4)));
        double ta = median_ms([&] { float s = sum_load(buf, n); keep(s); }, reps);
        double tu0 = median_ms([&] { float s = sum_loadu(buf, n); keep(s); }, reps);
        double tu4 = median_ms([&] { float s = sum_loadu(buf + 1, n); keep(s); }, reps);
        double gb = n * 4 / 1e6;
        std::printf("%-8zuКБ %8.1f    %10.1f     %10.1f       %.2fx\n", kb, gb / ta, gb / tu0, gb / tu4, tu4 / ta);
        std::free(buf);
    }
}
