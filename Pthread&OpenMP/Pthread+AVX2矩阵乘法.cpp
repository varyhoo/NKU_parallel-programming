#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <pthread.h>
#include <immintrin.h> // 包含 AVX2 指令集的头文件

using namespace std;

struct Multiply {
    const std::vector<std::vector<double>>* A;
    const std::vector<std::vector<double>>* B;
    std::vector<std::vector<double>>* C;
    int start_row;
    int end_row;
};
void* multiply_pthread_AVX2(void* args) {
    Multiply* multiply = static_cast<Multiply*>(args);
    const auto& A = *multiply->A;
    const auto& B = *multiply->B;
    auto& C = *multiply->C;

    int m = A.size();
    int n = A[0].size();
    int p = B[0].size();

    for (int i = multiply->start_row; i < multiply->end_row; i++) {
        for (int j = 0; j < p; j++) {
            __m256d sum_vec = _mm256_setzero_pd();
            for (int k = 0; k < n; k += 4) { 
                __m256d a_vec = _mm256_loadu_pd(&A[i][k]); 
                __m256d b_vec = _mm256_loadu_pd(&B[k][j]); 
                sum_vec = _mm256_add_pd(sum_vec, _mm256_mul_pd(a_vec, b_vec)); 
            }
            double sum[4];
            _mm256_storeu_pd(sum, sum_vec);
            C[i][j] = sum[0] + sum[1] + sum[2] + sum[3];
        }
    }
    return nullptr;
}

void create_martix(int a, vector<vector<double>>& A, vector<vector<double>>& B) // 修改参数为引用
{
    int b = a;
    int c = a;
    // 随机数生成器
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-10, 10);

    // 生成随机矩阵 A
    for (auto& row : A) {
        for (auto& elem : row) {
            elem = dis(gen);
        }
    }
    // 生成随机矩阵 B
    for (auto& row : B) {
        for (auto& elem : row) {
            elem = dis(gen);
        }
    }
}

int main() {
    int m;
    cin >> m;
    int n = m;
    int p = m;
    int num_threads = 4;

    vector<vector<double>> A(m, vector<double>(n));
    vector<vector<double>> B(n, vector<double>(p));
    vector<vector<double>> C(m, vector<double>(p, 0.0));
    create_martix(m, A, B);


    auto start = std::chrono::steady_clock::now();

    std::vector<pthread_t> threads(num_threads);
    std::vector<Multiply> thread_args(num_threads);
    int rows_per_thread = m / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_args[i] = { &A, &B, &C, i * rows_per_thread, (i + 1) * rows_per_thread };
        if (i == num_threads - 1) {
            thread_args[i].end_row = m;
        }
        pthread_create(&threads[i], nullptr, multiply_pthread_AVX2, &thread_args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "矩阵乘法完成" << std::endl;
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "计算耗时: " << elapsed.count() << " 秒" << std::endl;
    return 0;
}
