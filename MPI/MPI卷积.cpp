#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <mpi.h>

void performConvolution(const std::vector<float>& signal, const std::vector<float>& kernel, std::vector<float>& result, int rank, int worldSize) {
    int signal_length = signal.size();
    int kernel_length = kernel.size();
    int result_length = signal_length + kernel_length - 1;

    // 计算每个进程负责的工作范围
    int chunk_size = signal_length / worldSize;
    int start = rank * chunk_size;
    int end = (rank == worldSize - 1) ? signal_length : (rank + 1) * chunk_size;

    // 进行卷积计算
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < kernel_length; ++j) {
            int result_idx = i + j;
            if (result_idx < result_length) {
                result[result_idx] += signal[i] * kernel[j];
            }
        }
    }

    // 进行结果的全局归约
    MPI_Allreduce(MPI_IN_PLACE, result.data(), result_length, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
    // MPI初始化
    MPI_Init(&argc, &argv);

    // 获取MPI进程总数和当前进程编号
    int worldSize, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int signal_length=500000;
    int kernel_length = signal_length/10;

    // 每个进程的数据
    std::vector<float> signal(signal_length);
    std::vector<float> kernel(kernel_length);
    std::vector<float> result(signal_length + kernel_length - 1, 0.0);

    // 随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-10, 10);

    // 主进程生成随机信号和卷积核
    if (rank == 0) {
        // 生成随机信号
        for (auto& elem : signal) {
            elem = dis(gen);
        }

        // 生成随机卷积核
        for (auto& elem : kernel) {
            elem = dis(gen);
        }
    }

    // 将信号和卷积核广播给其他进程
    MPI_Bcast(signal.data(), signal_length, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(kernel.data(), kernel_length, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // 卷积操作计时开始
    auto startTime = std::chrono::high_resolution_clock::now();

    // 执行卷积操作
    performConvolution(signal, kernel, result, rank, worldSize);

    // 卷积操作计时结束
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;

    // 计算卷积操作的总时间
    double elapsedTimeValue = elapsedTime.count();
    double maxElapsedTime;
    MPI_Reduce(&elapsedTimeValue, &maxElapsedTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Total convolution time: " << maxElapsedTime << " seconds" << std::endl;
    }

    // MPI结束
    MPI_Finalize();

    return 0;
}
