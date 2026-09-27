// Фрагмент 6: скалярное произведение и Roofline-модель.
// Параметры заменены на параметры i5-12450H вместо i9-13900K из лекции:
//   pi   = 2 FMA-порта * 8 float * 2 FLOP * 4.4 ГГц = 140.8 GFLOPS (одно P-ядро, AVX2)
//   beta = 4800 МТ/с * 8 байт * 1 модуль DDR5       = 38.4 ГБ/с
// Медиана из 7 прогонов вместо среднего из 5, steady_clock вместо high_resolution_clock.
//
// Добавлен второй вариант dot_simd: float-накопитель и #pragma omp simd reduction,
// который разрешает компилятору векторизовать редукцию без -ffast-math
// (сборка с -fopenmp-simd).
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

double dot_product(const std::vector<float>& a, const std::vector<float>& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) sum += static_cast<double>(a[i]) * b[i];
    return sum;
}

float dot_simd(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0f;
    const float* pa = a.data();
    const float* pb = b.data();
    const size_t n = a.size();
#pragma omp simd reduction(+ : sum)
    for (size_t i = 0; i < n; ++i) sum += pa[i] * pb[i];
    return sum;
}

template <typename F>
double median_ms(F f, size_t n) {
    const int inner = static_cast<int>(std::max<size_t>(1, 20'000'000 / n));
    f();
    std::vector<double> times;
    for (int run = 0; run < 7; ++run) {
        auto t0 = std::chrono::steady_clock::now();
        for (int k = 0; k < inner; ++k) f();
        auto t1 = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count() / inner);
    }
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}

int main() {
    const double PEAK_FLOPS = 140.8e9;
    const double PEAK_BANDWIDTH = 38.4e9;
    const double RIDGE_POINT = PEAK_FLOPS / PEAK_BANDWIDTH;

    std::cout << "Пик: " << PEAK_FLOPS / 1e9 << " GFLOPS, память: " << PEAK_BANDWIDTH / 1e9
              << " ГБ/с, точка перегиба: " << std::setprecision(3) << RIDGE_POINT << " FLOP/байт\n";
    std::cout << "AI = 2n / 8n = 0.25 FLOP/байт, потолок Roofline = min(140.8, 38.4*0.25) = 9.60 GFLOPS\n\n";

    const std::vector<size_t> sizes = {1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000};
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    std::cout << std::left << std::setw(12) << "Размер"
              << std::setw(14) << "лекция, мс" << std::setw(14) << "лекция GF/s"
              << std::setw(14) << "simd, мс" << std::setw(14) << "simd GF/s"
              << std::setw(12) << "simd ГБ/с" << "\n" << std::string(80, '-') << "\n";

    volatile double sink = 0;
    for (size_t n : sizes) {
        std::vector<float> a(n), b(n);
        for (size_t i = 0; i < n; ++i) { a[i] = dis(gen); b[i] = dis(gen); }

        double t1 = median_ms([&] { sink = dot_product(a, b); }, n);
        double t2 = median_ms([&] { sink = dot_simd(a, b); }, n);
        double mflop = 2.0 * n / 1e6, mbyte = 8.0 * n / 1e6;

        std::cout << std::left << std::setw(12) << n << std::fixed
                  << std::setw(14) << std::setprecision(4) << t1
                  << std::setw(14) << std::setprecision(2) << mflop / t1
                  << std::setw(14) << std::setprecision(4) << t2
                  << std::setw(14) << std::setprecision(2) << mflop / t2
                  << std::setw(12) << mbyte / t2 << "\n";
    }
    return 0;
}
