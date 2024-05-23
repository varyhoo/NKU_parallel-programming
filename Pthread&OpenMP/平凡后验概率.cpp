//#include <iostream>  
//#include <vector>  
//#include <random>  
//#include <stdexcept>  
//#include <chrono>   
//
//void update_posterior(const std::vector<float>& likelihood, const std::vector<float>& prior, std::vector<float>& posterior) {
//    size_t n = likelihood.size();
//    size_t m = prior.size();
//    size_t p = posterior.size();
//
//    if (n != m || n != p) {
//        throw std::invalid_argument("Input vectors must have the same size.");
//    }
//
//    for (size_t i = 0; i < n; ++i) {
//        posterior[i] = likelihood[i] * prior[i];
//    }
//}
//
//int main() {
//    int N;
//    std::cin >> N;
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> dis(0.0f, 1.0f); // 生成[0, 1]之间的随机数  
//    //测量程序开始时间：
//    auto start = std::chrono::high_resolution_clock::now();
//    for (int i = 0; i < N; ++i) {
//        try {
//            size_t vector_size = 1 + gen() % 10; // 生成一个1到10之间的随机尺寸  
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
//            // 调用函数更新后验概率  
//            update_posterior(likelihood, prior, posterior);
//
//            // 在这里可以添加对后验概率的验证逻辑（如果需要的话）  
//
//            //std::cout << "Test " << i + 1 << " completed successfully." << std::endl;
//
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Test " << i + 1 << " failed: " << e.what() << std::endl;
//            return 1; // 如果发生异常，终止程序  
//        }
//    }
//    //测量程序结束时间
//    auto end = std::chrono::high_resolution_clock::now();
//
//    std::cout << "All " << N << " tests completed successfully." << std::endl;
//    std::chrono::duration<double> elapsed = end - start;
//    std::cout << "计算耗时: " << elapsed.count() << " 秒" << std::endl;
//    std::cout << "单次耗时" << (elapsed.count() / (double)N);
//    return 0;
//}