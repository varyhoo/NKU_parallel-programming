#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <immintrin.h> // 引入 AVX2 头文件

void performConvolution(const std::vector<float>& signal, const std::vector<float>& kernel, std::vector<float>& result) {
    int signal_length = signal.size();
    int kernel_length = kernel.size();
    int result_length = signal_length + kernel_length - 1;

    // 向量化计算
    for (int i = 0; i < signal_length; ++i) {
        __m256 sum = _mm256_setzero_ps(); // 初始化累加和为0

        for (int j = 0; j < kernel_length; j += 8) { // 使用 AVX2 指令每次处理 8 个元素
            __m256 signal_vec = _mm256_loadu_ps(&signal[i]);
            __m256 kernel_vec = _mm256_loadu_ps(&kernel[j]);
            __m256 result_vec = _mm256_mul_ps(signal_vec, kernel_vec);
            sum = _mm256_add_ps(sum, result_vec);
        }

        // 将累加和写入结果
        for (int k = 0; k < 8; ++k) {
            int result_idx = i + k;
            if (result_idx < result_length) {
                result[result_idx] += ((float*)&sum)[k];
            }
        }
    }
}

int main() {
    for (int i = 0; i < 10; i++)
    {
        int signal_length;
        std::cin >> signal_length;
        int kernel_length = signal_length / 10;

        // 生成随机数
        std::vector<float> signal(signal_length);
        std::vector<float> kernel(kernel_length);
        std::vector<float> result(signal_length + kernel_length - 1, 0.0);

        // 随机数生成器
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-10, 10);

        // 生成随机信号
        for (auto& elem : signal) {
            elem = dis(gen);
        }

        // 生成随机卷积核
        for (auto& elem : kernel) {
            elem = dis(gen);
        }

        // 卷积操作计时开始
        auto startTime = std::chrono::high_resolution_clock::now();

        // 执行卷积操作
        performConvolution(signal, kernel, result);

        // 卷积操作计时结束
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = endTime - startTime;

        // 输出卷积操作的总时间
        std::cout << "Total convolution time: " << elapsedTime.count() << " seconds" << std::endl;

    }

    return 0;
}
