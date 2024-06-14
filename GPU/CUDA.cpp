#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cuda_runtime.h>

// CUDA 核函数
__global__ void performConvolution(const float* signal, int signal_length, const float* kernel, int kernel_length, float* result) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < signal_length) {
        for (int j = 0; j < kernel_length; ++j) {
            int result_idx = i + j;
            if (result_idx < signal_length + kernel_length - 1) {
                atomicAdd(&result[result_idx], signal[i] * kernel[j]);
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

        // 在GPU上分配内存
        float* d_signal;
        float* d_kernel;
        float* d_result;
        cudaMalloc(&d_signal, signal_length * sizeof(float));
        cudaMalloc(&d_kernel, kernel_length * sizeof(float));
        cudaMalloc(&d_result, (signal_length + kernel_length - 1) * sizeof(float));

        // 将数据从主机传输到设备
        cudaMemcpy(d_signal, signal.data(), signal_length * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_kernel, kernel.data(), kernel_length * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_result, result.data(), (signal_length + kernel_length - 1) * sizeof(float), cudaMemcpyHostToDevice);

        // 定义块大小和网格大小
        int blockSize = 256;
        int numBlocks = (signal_length + blockSize - 1) / blockSize;

        // 卷积操作计时开始
        auto startTime = std::chrono::high_resolution_clock::now();

        // 执行CUDA核函数
        performConvolution << <numBlocks, blockSize >> > (d_signal, signal_length, d_kernel, kernel_length, d_result);
        cudaDeviceSynchronize();

        // 卷积操作计时结束
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = endTime - startTime;

        // 将结果从设备传输到主机
        cudaMemcpy(result.data(), d_result, (signal_length + kernel_length - 1) * sizeof(float), cudaMemcpyDeviceToHost);

        // 输出卷积操作的总时间
        std::cout << "Total convolution time: " << elapsedTime.count() << " seconds" << std::endl;

        // 释放GPU内存
        cudaFree(d_signal);
        cudaFree(d_kernel);
        cudaFree(d_result);
    }

    return 0;
}
