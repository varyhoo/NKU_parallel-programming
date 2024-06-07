#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <mpi.h>
#include <omp.h>
#include <immintrin.h> // ����AVX2ָ�

void performConvolution(const std::vector<float>& signal, const std::vector<float>& kernel, std::vector<float>& result, int rank, int worldSize) {
    int signal_length = signal.size();
    int kernel_length = kernel.size();
    int result_length = signal_length + kernel_length - 1;

    // ����ÿ�����̸���Ĺ�����Χ
    int chunk_size = signal_length / worldSize;
    int start = rank * chunk_size;
    int end = (rank == worldSize - 1) ? signal_length : (rank + 1) * chunk_size;

    // OpenMP���л��������
#pragma omp parallel for
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < kernel_length; j += 8) { // ʹ��AVX2һ�δ���8��Ԫ��
            int result_idx = i + j;
            if (result_idx < result_length) {
                // ʹ��AVX2�����źź;����
                __m256 signal_val = _mm256_set1_ps(signal[i]);
                __m256 kernel_val = _mm256_loadu_ps(&kernel[j]);
                __m256 result_val = _mm256_loadu_ps(&result[result_idx]);

                // ������
                result_val = _mm256_fmadd_ps(signal_val, kernel_val, result_val);

                // �洢���
                _mm256_storeu_ps(&result[result_idx], result_val);
            }
        }
    }

    // ���н����ȫ�ֹ�Լ
    MPI_Allreduce(MPI_IN_PLACE, result.data(), result_length, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
    // MPI��ʼ��
    MPI_Init(&argc, &argv);

    // ��ȡMPI���������͵�ǰ���̱��
    int worldSize, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int signal_length = 500000;
    int kernel_length = signal_length / 10;

    // ÿ�����̵�����
    std::vector<float> signal(signal_length);
    std::vector<float> kernel(kernel_length);
    std::vector<float> result(signal_length + kernel_length - 1, 0.0f);

    // �����������
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-10, 10);

    // ��������������źź;����
    if (rank == 0) {
        // ��������ź�
        for (auto& elem : signal) {
            elem = dis(gen);
        }

        // ������������
        for (auto& elem : kernel) {
            elem = dis(gen);
        }
    }

    // ���źź;���˹㲥����������
    MPI_Bcast(signal.data(), signal_length, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(kernel.data(), kernel_length, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // ���������ʱ��ʼ
    auto startTime = std::chrono::high_resolution_clock::now();

    // ִ�о������
    performConvolution(signal, kernel, result, rank, worldSize);

    // ���������ʱ����
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;

    // ��������������ʱ��
    double elapsedTimeValue = elapsedTime.count();
    double maxElapsedTime;
    MPI_Reduce(&elapsedTimeValue, &maxElapsedTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Total convolution time: " << maxElapsedTime << " seconds" << std::endl;
    }

    // MPI����
    MPI_Finalize();

    return 0;
}
