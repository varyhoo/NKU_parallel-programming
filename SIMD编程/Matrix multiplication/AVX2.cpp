#include <iostream> 
#include <vector>  
#include <immintrin.h> 
#include <chrono>    
#include <random>  

// ʹ��AVX2ָ����о���˷�  
void matrix_multiply_avx2(const std::vector<float>& a, const std::vector<float>& b, std::vector<float>& c, int m, int n, int p) {
    for (int i = 0; i < m; ++i) {  
        for (int j = 0; j < p; j += 8) {
            // ��ʼ���ۼӺ�Ϊ0  
            __m256 c_line = _mm256_setzero_ps();
            for (int k = 0; k < n; ++k) {
                // ������A�ĵ�ǰԪ�ع㲥��AVX2�Ĵ���  
                __m256 a_elem = _mm256_set1_ps(a[i * n + k]);
                // �Ӿ���B�м���һ�У�8��Ԫ�أ���AVX2�Ĵ���  
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
        int m; // ����A������  
        std::cin >> m;
        int n = m; // ����A��������Ҳ�Ǿ���B������  
        int p = m; // ����B��������Ҳ�Ǿ���C������  

        // ������̬�������ڴ洢����A��B��C  
        std::vector<float> a(m * n);
        std::vector<float> b(n * p);
        std::vector<float> c(m * p);

        // ���������������  
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-10, 10); // �ֲ���Χ��-10��10֮��  

        // �����������A  
        for (int i = 0; i < m * n; ++i) {
            a[i] = dis(gen);
        }

        // �����������B  
        for (int i = 0; i < n * p; ++i) {
            b[i] = dis(gen);
        }

        // ��������˷���ʼʱ��  
        auto start = std::chrono::high_resolution_clock::now();

        // ִ�о���˷�  
        matrix_multiply_avx2(a, b, c, m, n, p);

        // ��������˷�����ʱ��  
        auto end = std::chrono::high_resolution_clock::now();

        // ������  
        std::cout << "����˷����" << std::endl;
        // ���㲢���ִ��ʱ��  
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "�����ʱ: " << elapsed.count() << " ��" << std::endl;

      
    }
    return 0;
}