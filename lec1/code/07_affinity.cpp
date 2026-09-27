// Фрагмент 5: Привязка потоков к ядрам процессора (Thread Affinity) на Linux
#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include <pthread.h>
#include <sched.h>

std::mutex out_mutex;

void set_thread_affinity(int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "Ошибка привязки потока к ядру " << core_id << "\n";
    }
}

void thread_function(int thread_id) {
    set_thread_affinity(thread_id);
    int current_core = sched_getcpu();
    std::lock_guard<std::mutex> lock(out_mutex);
    std::cout << "Поток " << thread_id << " привязан к ядру " << current_core << "\n";
}

int main() {
    int num_cores = std::thread::hardware_concurrency();
    std::cout << "Доступно логических ядер: " << num_cores << "\n\n";

    std::vector<std::thread> threads;
    int num_threads = std::min(num_cores, 8);
    for (int i = 0; i < num_threads; ++i) threads.emplace_back(thread_function, i);
    for (auto& t : threads) t.join();
    return 0;
}
