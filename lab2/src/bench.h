// Общие утилиты замеров для лабораторной 2.
#pragma once
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <vector>

// Медиана времени (в секундах) из runs вызовов f() после одного прогревочного.
template <typename F>
double median_s(F&& f, int runs) {
    std::vector<double> t;
    f();
    for (int r = 0; r < runs; ++r) {
        auto t0 = std::chrono::steady_clock::now();
        f();
        auto t1 = std::chrono::steady_clock::now();
        t.push_back(std::chrono::duration<double>(t1 - t0).count());
    }
    std::sort(t.begin(), t.end());
    return t[t.size() / 2];
}

inline double max_abs_diff(const float* a, const float* b, size_t n) {
    double e = 0;
    for (size_t i = 0; i < n; ++i) e = std::max(e, double(std::fabs(a[i] - b[i])));
    return e;
}

// Число повторов: меньше для больших размеров, чтобы весь прогон укладывался в минуты.
inline int runs_for(int n) { return n <= 512 ? 10 : (n <= 1024 ? 5 : 3); }
