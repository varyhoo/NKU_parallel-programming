//#include <iostream>
//#include <vector>
//#include <random>
//#include <stdexcept>
//#include <chrono>
//#include <pthread.h>
//#include <immintrin.h> // 包含 AVX2 指令集的头文件
//
//const int NUM_THREADS = 4;
//
//// 定义线程数据结构
//struct ThreadData {
//    int start;
//    int end;
//    const std::vector<float>* likelihood;
//    const std::vector<float>* prior;
//    std::vector<float>* posterior;
//};
//
//void* updatePosterior(void* arg) {
//    ThreadData* data = static_cast<ThreadData*>(arg);
//    const std::vector<float>& likelihood = *data->likelihood;
//    const std::vector<float>& prior = *data->prior;
//    std::vector<float>& posterior = *data->posterior;
//
//    size_t vector_size = sizeof(__m256) / sizeof(float);
//
//    for (int i = data->start; i < data->end; ++i) {
//        size_t index = i * vector_size;
//
//        __m256 likeli_vec = _mm256_loadu_ps(&likelihood[index]);
//        __m256 prior_vec = _mm256_loadu_ps(&prior[index]);
//        __m256 result_vec = _mm256_mul_ps(likeli_vec, prior_vec);
//        _mm256_storeu_ps(&posterior[index], result_vec);
//    }
//
//    pthread_exit(NULL);
//    return nullptr;
//}
//
//int main() {
//    int N;
//    std::cin >> N;
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> dis(0.0f, 1.0f);
//
//    auto start = std::chrono::high_resolution_clock::now();
//
//    for (int i = 0; i < N; ++i) {
//        size_t vector_size = 1 + gen() % 10;
//
//        std::vector<float> likelihood(vector_size);
//        std::vector<float> prior(vector_size);
//        std::vector<float> posterior(vector_size);
//
//        for (size_t j = 0; j < vector_size; ++j) {
//            likelihood[j] = dis(gen);
//            prior[j] = dis(gen);
//        }
//
//        // 启动多个线程进行计算
//        pthread_t threads[NUM_THREADS];
//        ThreadData thread_data[NUM_THREADS];
//        int chunk_size = vector_size / NUM_THREADS;
//        int remainder = vector_size % NUM_THREADS;
//        int start = 0;
//
//        for (int t = 0; t < NUM_THREADS; ++t) {
//            thread_data[t].start = start;
//            thread_data[t].end = start + chunk_size + (t < remainder ? 1 : 0);
//            thread_data[t].likelihood = &likelihood;
//            thread_data[t].prior = &prior;
//            thread_data[t].posterior = &posterior;
//            start = thread_data[t].end;
//            pthread_create(&threads[t], NULL, updatePosterior, &thread_data[t]);
//        }
//
//        // 等待所有线程完成
//        for (int t = 0; t < NUM_THREADS; ++t) {
//            pthread_join(threads[t], NULL);
//        }
//    }
//
//
//    auto end = std::chrono::high_resolution_clock::now();
//
//    std::cout << "All " << N << " tests completed successfully." << std::endl;
//    std::chrono::duration<double> elapsed = end - start;
//    std::cout << "计算耗时: " << elapsed.count() << " 秒" << std::endl;
//    std::cout << "单次耗时" << (elapsed.count() / (double)N);
//
//    return 0;
//}
