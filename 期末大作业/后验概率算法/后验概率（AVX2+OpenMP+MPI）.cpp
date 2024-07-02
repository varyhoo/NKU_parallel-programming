#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>
#include <chrono>
#include <omp.h>
#include <immintrin.h>
#include <mpi.h>

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

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int N;
    if (world_rank == 0) {
        std::cin >> N;
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f); // 生成[0, 1]之间的随机数

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = world_rank; i < N; i += world_size) {
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
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    double local_elapsed = elapsed.count;
    double total_elapsed;

    MPI_Reduce(&local_elapsed, &total_elapsed, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        std::cout << "All " << N << " tests completed successfully." << std::endl;
        std::cout << "总耗时: " << total_elapsed << " 秒" << std::endl;
        std::cout << "每次测试平均耗时: " << (total_elapsed / static_cast<double>(N)) << " 秒" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
