#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>
#include <chrono>
#include <omp.h>
#include <immintrin.h>

void update_posterior(const std::vector<float>& likelihood, const std::vector<float>& prior, std::vector<float>& posterior) {
    size_t n = likelihood.size();

    if (n != prior.size() || n != posterior.size()) {
        throw std::invalid_argument("Input vectors must have the same size.");
    }

    #pragma omp parallel for
    for (size_t i = 0; i < n; i += 8) {
        __m256 likelihood_vec = _mm256_loadu_ps(&likelihood[i]);
        __m256 prior_vec = _mm256_loadu_ps(&prior[i]);
        __m256 posterior_vec = _mm256_mul_ps(likelihood_vec, prior_vec);
        _mm256_storeu_ps(&posterior[i], posterior_vec);
    }

    // Handle remaining elements if n is not a multiple of 8
    for (size_t i = (n / 8) * 8; i < n; ++i) {
        posterior[i] = likelihood[i] * prior[i];
    }
}

int main() {
    int N;
    std::cin >> N;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f); // 生成[0, 1]之间的随机数
    // 获取开始时间：
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        try {
            size_t vector_size = 1 + gen() % 10; // 生成一个1到10之间的随机大小

            std::vector<float> likelihood(vector_size);
            std::vector<float> prior(vector_size);
            std::vector<float> posterior(vector_size);

            // 填充随机数
            for (size_t j = 0; j < vector_size; ++j) {
                likelihood[j] = dis(gen);
                prior[j] = dis(gen);
            }

            // 调用更新后验概率函数
            update_posterior(likelihood, prior, posterior);

        } catch (const std::exception& e) {
            std::cerr << "Test " << i + 1 << " failed: " << e.what() << std::endl;
            return 1; // 如果捕获到异常，则停止执行
        }
    }

    // 获取结束时间
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "All " << N << " tests completed successfully." << std::endl;
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "总耗时: " << elapsed.count() << " 秒" << std::endl;
    std::cout << "每次测试平均耗时: " << (elapsed.count() / static_cast<double>(N)) << " 秒" << std::endl;
    return 0;
}
