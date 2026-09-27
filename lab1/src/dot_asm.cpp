// Задача 3: код для Compiler Explorer (godbolt.org) и локального g++ -S.
// Компилятор: x86-64 gcc 11.2 или новее
// Флаги: -O0, -O3, -O3 -march=native, -O3 -march=native -ffast-math

float dot_product_basic(const float* a, const float* b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

// Вариант с __restrict__, помогающий компилятору доказать отсутствие алиасинга
float dot_product_restrict(const float* __restrict__ a, const float* __restrict__ b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}
