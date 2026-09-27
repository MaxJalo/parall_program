// Фрагмент 1: Расчёт ускорения по законам Амдала и Густафсона
#include <iostream>
#include <iomanip>
#include <vector>

// Закон Амдала: S = 1 / ((1 - P) + P/N)
// P - доля параллелизуемого кода, N - количество ядер
double amdahls_law(double P, int N) {
    return 1.0 / ((1.0 - P) + P / static_cast<double>(N));
}

// Закон Густафсона: S = N - p*(N-1), где p - доля последовательного кода
double gustafsons_law(double P, int N) {
    double p = 1.0 - P;
    return N - p * (N - 1);
}

int main() {
    const double P = 0.95;
    const std::vector<int> cores = {1, 2, 4, 8, 16, 32, 64, 128};

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Доля параллельного кода: " << P * 100 << "%\n\n";
    std::cout << std::left << std::setw(10) << "Ядра"
              << std::setw(15) << "Амдал"
              << std::setw(15) << "Густафсон" << "\n";
    std::cout << std::string(40, '-') << "\n";

    for (int N : cores) {
        std::cout << std::left << std::setw(10) << N
                  << std::setw(15) << amdahls_law(P, N)
                  << std::setw(15) << gustafsons_law(P, N) << "\n";
    }

    std::cout << "\nПредельное ускорение (Амдал, N -> inf): " << 1.0 / (1.0 - P) << "\n";
    return 0;
}
