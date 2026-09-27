// Фрагменты 3 и 4: ложное совместное использование (false sharing)
// в одной программе, чтобы сравнить оба варианта в одинаковых условиях.
//
// Отличие от лекции: инкремент идёт через volatile-ссылку. Без этого g++ -O2
// держит счётчик в регистре и пишет в память один раз в конце цикла,
// и false sharing просто исчезает — сравнивать становится нечего.
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

struct alignas(64) PaddedCounter {
    long long value = 0;
};

template <typename Counter, typename Get>
double run(std::vector<Counter>& counters, int threads_n, int iters, Get get) {
    auto start = std::chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (int t = 0; t < threads_n; ++t) {
        threads.emplace_back([&, t] {
            volatile long long& c = get(counters[t]);
            for (int i = 0; i < iters; ++i) c = c + 1;
        });
    }
    for (auto& th : threads) th.join();
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
}

int main() {
    // в лекции 8 потоков; берём не больше числа логических CPU, иначе потоки
    // вытесняют друг друга и эффект смазывается
    const int NUM_THREADS = std::min(8u, std::thread::hardware_concurrency());
    const int ITERATIONS = 100'000'000;

    std::vector<long long> plain(NUM_THREADS, 0);
    std::vector<PaddedCounter> padded(NUM_THREADS);

    std::cout << "sizeof(long long) = " << sizeof(long long)
              << ", sizeof(PaddedCounter) = " << sizeof(PaddedCounter) << "\n";
    std::cout << "Потоков: " << NUM_THREADS << ", итераций на поток: " << ITERATIONS << "\n\n";

    double t_plain = run(plain, NUM_THREADS, ITERATIONS, [](long long& c) -> long long& { return c; });
    double t_padded = run(padded, NUM_THREADS, ITERATIONS, [](PaddedCounter& c) -> long long& { return c.value; });

    long long s1 = 0, s2 = 0;
    for (int i = 0; i < NUM_THREADS; ++i) { s1 += plain[i]; s2 += padded[i].value; }

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Без выравнивания (все счётчики в одной линии): " << t_plain << " мс, сумма " << s1 << "\n";
    std::cout << "С alignas(64) (по счётчику на линию):         " << t_padded << " мс, сумма " << s2 << "\n";
    std::cout << "Замедление из-за false sharing: " << std::setprecision(2) << t_plain / t_padded << "x\n";
    return 0;
}
