#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <immintrin.h> // ���� AVX2 ͷ�ļ�

void performConvolution(const std::vector<float>& signal, const std::vector<float>& kernel, std::vector<float>& result) {
    int signal_length = signal.size();
    int kernel_length = kernel.size();
    int result_length = signal_length + kernel_length - 1;

    // ����������
    for (int i = 0; i < signal_length; ++i) {
        __m256 sum = _mm256_setzero_ps(); // ��ʼ���ۼӺ�Ϊ0

        for (int j = 0; j < kernel_length; j += 8) { // ʹ�� AVX2 ָ��ÿ�δ��� 8 ��Ԫ��
            __m256 signal_vec = _mm256_loadu_ps(&signal[i]);
            __m256 kernel_vec = _mm256_loadu_ps(&kernel[j]);
            __m256 result_vec = _mm256_mul_ps(signal_vec, kernel_vec);
            sum = _mm256_add_ps(sum, result_vec);
        }

        // ���ۼӺ�д����
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

        // ���������
        std::vector<float> signal(signal_length);
        std::vector<float> kernel(kernel_length);
        std::vector<float> result(signal_length + kernel_length - 1, 0.0);

        // �����������
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-10, 10);

        // ��������ź�
        for (auto& elem : signal) {
            elem = dis(gen);
        }

        // ������������
        for (auto& elem : kernel) {
            elem = dis(gen);
        }

        // ���������ʱ��ʼ
        auto startTime = std::chrono::high_resolution_clock::now();

        // ִ�о������
        performConvolution(signal, kernel, result);

        // ���������ʱ����
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = endTime - startTime;

        // ��������������ʱ��
        std::cout << "Total convolution time: " << elapsedTime.count() << " seconds" << std::endl;

    }

    return 0;
}
