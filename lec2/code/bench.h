// Общая утилита замеров: медиана времени нескольких прогонов.
#pragma once
#include <algorithm>
#include <chrono>
#include <vector>

// Выполняет f() reps раз подряд, повторяет это runs раз и возвращает медиану времени одного вызова в мс.
template <typename F>
double median_ms(F&& f, int reps = 1, int runs = 7) {
    std::vector<double> t;
    f();  // прогрев: кэши, страницы памяти
    for (int r = 0; r < runs; ++r) {
        auto t0 = std::chrono::steady_clock::now();
        for (int k = 0; k < reps; ++k) f();
        auto t1 = std::chrono::steady_clock::now();
        t.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count() / reps);
    }
    std::sort(t.begin(), t.end());
    return t[t.size() / 2];
}

// Не даёт компилятору выкинуть вычисление, результат которого не используется.
template <typename T>
inline void keep(const T& v) {
    asm volatile("" : : "g"(&v) : "memory");
}
