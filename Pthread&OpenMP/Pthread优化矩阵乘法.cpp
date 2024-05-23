//#include <iostream>
//#include <vector>
//#include <chrono>
//#include <random>
//#include <pthread.h>
//using namespace std;
//struct Multiply {
//    const std::vector<std::vector<double>>* A;
//    const std::vector<std::vector<double>>* B;
//    std::vector<std::vector<double>>* C;
//    int start_row;
//    int end_row;
//};
//
//void* multiply_pthread(void* args) {
//    Multiply* multiply = static_cast<Multiply*>(args);
//    const auto& A = *multiply->A;
//    const auto& B = *multiply->B;
//    auto& C = *multiply->C;
//
//    int m = A.size();
//    int n = A[0].size();
//    int p = B[0].size();
//
//    for (int i = multiply->start_row; i < multiply->end_row; i++) {
//        for (int j = 0; j < p; j++) {
//            for (int k = 0; k < n; k++) {
//                C[i][j] += A[i][k] * B[k][j];
//            }
//        }
//    }
//    return nullptr;
//}
//void create_martix(int a,vector<vector<double>>A, vector<vector<double>>B)
//{
//    int b = a;
//    int c = a;
//    // 随机数生成器
//    random_device rd;
//    mt19937 gen(rd());
//    uniform_real_distribution<> dis(-10, 10);
//
//    // 生成随机矩阵 A
//    for (auto& row : A) {
//        for (auto& elem : row) {
//            elem = dis(gen);
//        }
//    }
//    // 生成随机矩阵 B
//    for (auto& row : B) {
//        for (auto& elem : row) {
//            elem = dis(gen);
//        }
//    }
//}
//int main() {
//    int m ;
//    cin >> m;
//    int n = m;
//    int p = m;
//    int num_threads = 4;
//
//    vector<vector<double>> A(m, vector<double>(n));
//    vector<vector<double>> B(n, vector<double>(p));
//    vector<vector<double>> C(m, vector<double>(p, 0.0));
//    create_martix(m, A, B);
//    
//
//    auto start = std::chrono::steady_clock::now();
//
//    std::vector<pthread_t> threads(num_threads);
//    std::vector<Multiply> thread_args(num_threads);
//    int rows_per_thread = m / num_threads;
//
//    for (int i = 0; i < num_threads; i++) {
//        thread_args[i] = { &A, &B, &C, i * rows_per_thread, (i + 1) * rows_per_thread };
//        if (i == num_threads - 1) {
//            thread_args[i].end_row = m;
//        }
//        pthread_create(&threads[i], nullptr, multiply_pthread, &thread_args[i]);
//    }
//
//    for (int i = 0; i < num_threads; i++) {
//        pthread_join(threads[i], nullptr);
//    }
//    auto end = std::chrono::steady_clock::now();
//    std::cout << "矩阵乘法完成" << std::endl;
//    std::chrono::duration<double> elapsed = end - start;
//    std::cout << "计算耗时: " << elapsed.count() << " 秒" << std::endl;
//    return 0;
//}
