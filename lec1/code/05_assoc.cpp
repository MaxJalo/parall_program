// Конфликтные промахи (3C: conflict miss) из-за ограниченной ассоциативности L1d.
// L1d i5-12450H: 48 КБ, 12-way, линия 64 Б -> 48K / (12 * 64) = 64 набора.
// Адреса с шагом 64 * 64 = 4096 Б попадают в ОДИН набор: больше 12 таких линий
// в L1 не помещаются, хотя суммарно это всего несколько килобайт.
// Шаг 4096 + 64 раскладывает те же линии по разным наборам.
//
// Замер — «погоня за указателями»: каждая следующая загрузка зависит от предыдущей,
// поэтому время одного шага = задержка того уровня кэша, где лежит линия
// (L1 ~5 тактов, L2 ~15 тактов).
#include <chrono>
#include <cstdio>
#include <vector>

double chase_ns(size_t stride, int lines) {
    std::vector<char> buf(stride * lines + 64);
    for (int k = 0; k < lines; ++k) {
        char* here = &buf[k * stride];
        char* next = &buf[((k + 1) % lines) * stride];
        *reinterpret_cast<char**>(here) = next;
    }
    const long long steps = 100'000'000;
    char* p = &buf[0];
    auto t0 = std::chrono::steady_clock::now();
    for (long long s = 0; s < steps; ++s) {
        p = *reinterpret_cast<char**>(p);
        asm volatile("" : "+r"(p));  // запрет компилятору выкидывать цепочку загрузок
    }
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::nano>(t1 - t0).count() / steps;
}

int main() {
    std::printf("%-8s %-16s %-16s\n", "линий", "шаг 4096 (нс)", "шаг 4160 (нс)");
    for (int lines : {4, 8, 12, 13, 16, 24, 32, 64}) {
        std::printf("%-8d %-16.3f %-16.3f\n", lines, chase_ns(4096, lines), chase_ns(4096 + 64, lines));
    }
    return 0;
}
