#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <omp.h>

void performConvolution(const std::vector<float>& signal, const std::vector<float>& kernel, std::vector<float>& result) {
    int signal_length = signal.size();
    int kernel_length = kernel.size();
    int result_length = signal_length + kernel_length - 1;

    // 并行化卷积计算
#pragma omp parallel for
    for (int i = 0; i < signal_length; ++i) {
        for (int j = 0; j < kernel_length; ++j) {
            int result_idx = i + j;
            if (result_idx < result_length) {
#pragma omp atomic
                result[result_idx] += signal[i] * kernel[j];
            }
        }
    }
}

int main() {
    for (int i = 0; i < 10; i++) {
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
