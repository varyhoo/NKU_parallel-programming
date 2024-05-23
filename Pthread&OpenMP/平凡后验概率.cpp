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
//    std::uniform_real_distribution<> dis(0.0f, 1.0f); // ����[0, 1]֮��������  
//    //��������ʼʱ�䣺
//    auto start = std::chrono::high_resolution_clock::now();
//    for (int i = 0; i < N; ++i) {
//        try {
//            size_t vector_size = 1 + gen() % 10; // ����һ��1��10֮�������ߴ�  
//
//            std::vector<float> likelihood(vector_size);
//            std::vector<float> prior(vector_size);
//            std::vector<float> posterior(vector_size);
//
//            // �������������Ȼ����  
//            for (size_t j = 0; j < vector_size; ++j) {
//                likelihood[j] = dis(gen);
//                prior[j] = dis(gen);
//            }
//
//            // ���ú������º������  
//            update_posterior(likelihood, prior, posterior);
//
//            // �����������ӶԺ�����ʵ���֤�߼��������Ҫ�Ļ���  
//
//            //std::cout << "Test " << i + 1 << " completed successfully." << std::endl;
//
//        }
//        catch (const std::exception& e) {
//            std::cerr << "Test " << i + 1 << " failed: " << e.what() << std::endl;
//            return 1; // ��������쳣����ֹ����  
//        }
//    }
//    //�����������ʱ��
//    auto end = std::chrono::high_resolution_clock::now();
//
//    std::cout << "All " << N << " tests completed successfully." << std::endl;
//    std::chrono::duration<double> elapsed = end - start;
//    std::cout << "�����ʱ: " << elapsed.count() << " ��" << std::endl;
//    std::cout << "���κ�ʱ" << (elapsed.count() / (double)N);
//    return 0;
//}