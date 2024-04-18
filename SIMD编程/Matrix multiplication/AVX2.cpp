#include <iostream> 
#include <vector>  
#include <immintrin.h> 
#include <chrono>    
#include <random>  

// 使用AVX2指令集进行矩阵乘法  
void matrix_multiply_avx2(const std::vector<float>& a, const std::vector<float>& b, std::vector<float>& c, int m, int n, int p) {
    for (int i = 0; i < m; ++i) {  
        for (int j = 0; j < p; j += 8) {
            // 初始化累加和为0  
            __m256 c_line = _mm256_setzero_ps();
            for (int k = 0; k < n; ++k) {
                // 将矩阵A的当前元素广播到AVX2寄存器  
                __m256 a_elem = _mm256_set1_ps(a[i * n + k]);
                // 从矩阵B中加载一行（8个元素）到AVX2寄存器  
                __m256 b_line = _mm256_loadu_ps(&b[k * p + j]);
                c_line = _mm256_add_ps(c_line, _mm256_mul_ps(a_elem, b_line));
            } 
            _mm256_storeu_ps(&c[i * p + j], c_line);
        }
    }
}

int main() {
    for (int i = 0; i < 10; i++)
    {
        int m; // 矩阵A的行数  
        std::cin >> m;
        int n = m; // 矩阵A的列数，也是矩阵B的行数  
        int p = m; // 矩阵B的列数，也是矩阵C的列数  

        // 创建动态数组用于存储矩阵A、B和C  
        std::vector<float> a(m * n);
        std::vector<float> b(n * p);
        std::vector<float> c(m * p);

        // 创建随机数生成器  
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-10, 10); // 分布范围在-10到10之间  

        // 生成随机矩阵A  
        for (int i = 0; i < m * n; ++i) {
            a[i] = dis(gen);
        }

        // 生成随机矩阵B  
        for (int i = 0; i < n * p; ++i) {
            b[i] = dis(gen);
        }

        // 测量矩阵乘法开始时间  
        auto start = std::chrono::high_resolution_clock::now();

        // 执行矩阵乘法  
        matrix_multiply_avx2(a, b, c, m, n, p);

        // 测量矩阵乘法结束时间  
        auto end = std::chrono::high_resolution_clock::now();

        // 输出结果  
        std::cout << "矩阵乘法完成" << std::endl;
        // 计算并输出执行时间  
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "计算耗时: " << elapsed.count() << " 秒" << std::endl;

      
    }
    return 0;
}