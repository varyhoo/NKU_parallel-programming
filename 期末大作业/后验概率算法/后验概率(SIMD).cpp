#include <iostream>  
#include <vector>  
#include <chrono>  
#include <random>  
#include <cassert>  
#include <immintrin.h> // ����SIMDָ�ͷ�ļ�  

void update_posterior(const std::vector<float>& likelihood, const std::vector<float>& prior, std::vector<float>& posterior)
{
    assert(likelihood.size() == prior.size() && likelihood.size() == posterior.size());
    size_t n = likelihood.size();
    size_t aligned_n = n & ~7; // ȷ���������������8�ı���
    for (size_t i = 0; i < aligned_n; i += 8)
    {
        __m256 likelihoods = _mm256_loadu_ps(&likelihood[i]);
        __m256 priors = _mm256_loadu_ps(&prior[i]);
        __m256 posters = _mm256_mul_ps(likelihoods, priors); // ����������
        _mm256_storeu_ps(&posterior[i], posters); // �洢���
    } // ����ʣ���Ԫ�أ�����еĻ���
    for (size_t i = aligned_n; i < n; ++i)
    {
        posterior[i] = likelihood[i] * prior[i];
    }
}

int main() {
    for (int i = 0; i < 10; i++)
    {
        size_t N;
        std::cout << "Enter the size of data (N): ";
        std::cin >> N;
        std::cin.ignore(); // �����κ�ʣ�������  

        // �����������  
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0f, 1.0f);

        std::vector<float> likelihood(N);
        std::vector<float> prior(N);
        std::vector<float> posterior(N);

        for (size_t i = 0; i < N; ++i) {
            likelihood[i] = dis(gen);
            prior[i] = dis(gen);
        }

        // ��������ʱ��  
        auto start = std::chrono::high_resolution_clock::now();
        update_posterior(likelihood, prior, posterior);
        auto end = std::chrono::high_resolution_clock::now();

        // ���㲢�������ʱ��  
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Time taken by update_posterior: " << elapsed.count() * (double)100 << " seconds" << std::endl;
    }
   

    return 0;
}