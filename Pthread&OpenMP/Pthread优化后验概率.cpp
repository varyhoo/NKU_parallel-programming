//#include <iostream>  
//#include <vector>  
//#include <random>  
//#include <stdexcept>  
//#include <chrono>  
//#include <pthread.h>  
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
//// 线程函数  
//void* update_posterior_thread(void* arg) {
//    ThreadData* data = static_cast<ThreadData*>(arg);
//    for (int i = data->start; i < data->end; ++i) {
//        data->posterior->at(i) = data->likelihood->at(i) * data->prior->at(i);
//    }
//    return nullptr;
//}
//
//// 主函数中的update_posterior逻辑现在由线程执行  
//void update_posterior_parallel(const std::vector<float>& likelihood, const std::vector<float>& prior, std::vector<float>& posterior, int num_threads) {
//    if (likelihood.size() != prior.size() || likelihood.size() != posterior.size()) {
//        throw std::invalid_argument("Input vectors must have the same size.");
//    }
//
//    // 分配任务给线程  
//    int vector_size = likelihood.size();
//    int chunk_size = vector_size / num_threads;
//    pthread_t* threads=new pthread_t[num_threads];
//    ThreadData* thread_data=new ThreadData[num_threads];
//
//    for (int i = 0; i < num_threads; ++i) {
//        int start = i * chunk_size;
//        int end = (i == num_threads - 1) ? vector_size : start + chunk_size;
//
//        thread_data[i].start = start;
//        thread_data[i].end = end;
//        thread_data[i].likelihood = &likelihood;
//        thread_data[i].prior = &prior;
//        thread_data[i].posterior = &posterior;
//
//        pthread_create(&threads[i], nullptr, update_posterior_thread, &thread_data[i]);
//    }
//
//    // 等待所有线程完成  
//    for (int i = 0; i < num_threads; ++i) {
//        pthread_join(threads[i], nullptr);
//    }
//}
//
//int main() {
//    int N;
//    std::cin >> N;
//    int num_threads = 4; 
//
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> dis(0.0f, 1.0f);
//
//    // 测量程序开始时间  
//    auto start = std::chrono::high_resolution_clock::now();
//
//    for (int test_iter = 0; test_iter < N; ++test_iter) {
//            size_t vector_size = 1 + gen() % 10;
//
//            std::vector<float> likelihood(vector_size);
//            std::vector<float> prior(vector_size);
//            std::vector<float> posterior(vector_size);
//
//            // 填充随机先验和似然概率  
//            for (size_t j = 0; j < vector_size; ++j) {
//                likelihood[j] = dis(gen);
//                prior[j] = dis(gen);
//            }
//
//            // 使用线程并行更新后验概率  
//            update_posterior_parallel(likelihood, prior, posterior, num_threads);
//
//            // 在这里可以添加对后验概率的验证逻辑（如果需要的话）  
//
//            // std::cout << "Test " << test_iter + 1 << " completed successfully." << std::endl;  
//
//    }
//        //测量程序结束时间
//        auto end = std::chrono::high_resolution_clock::now();
//    
//        std::cout << "All " << N << " tests completed successfully." << std::endl;
//        std::chrono::duration<double> elapsed = end - start;
//        std::cout << "计算耗时: " << elapsed.count() << " 秒" ;
//}
//
