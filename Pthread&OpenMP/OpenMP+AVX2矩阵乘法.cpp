#include <iostream>
#include <vector>
#include <immintrin.h>
#include <chrono>
#include <random>
#include <omp.h>
using namespace std;
const int num_threads = 4;
void multiply_openmp_avx2(vector<float>& a, vector<float>&b, vector<float>&c,int m)
{
    // 设置线程数
    omp_set_num_threads(num_threads);

#pragma omp parallel for
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; j += 8) {
            __m256 c_line = _mm256_setzero_ps();
            for (int k = 0; k < m; ++k) {
                __m256 a_elem = _mm256_set1_ps(a[i * m + k]);
                __m256 b_line = _mm256_loadu_ps(&b[k * m + j]);
                c_line = _mm256_add_ps(c_line, _mm256_mul_ps(a_elem, b_line));
            }
            _mm256_storeu_ps(&c[i * m + j], c_line);
        }
    }
}
int main() {
    int m ;
    cin >> m;
    int n = m;
    int p = n;


    std::vector<float> a(m * n);
    std::vector<float> b(n * p);
    std::vector<float> c(m * p);
    auto start = std::chrono::high_resolution_clock::now();
    // 随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-10, 10);

    // 生成随机矩阵 A
    for (int i = 0; i < m * n; ++i) {
        a[i] = dis(gen);
    }

    // 生成随机矩阵 B
    for (int i = 0; i < n * p; ++i) {
        b[i] = dis(gen);
    }
    multiply_openmp_avx2(a, b, c,m);
 

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "矩阵乘法完成" << std::endl;
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "计算耗时: " << elapsed.count() << " 秒" << std::endl;
    return 0;
}
