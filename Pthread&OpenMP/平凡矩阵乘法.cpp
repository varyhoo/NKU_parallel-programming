//#include <iostream>  
//#include <ctime>  
//#include <cstdlib>  
//#include<chrono>
//
//// 函数用于矩阵乘法  
//int** matrix_multiply(int** A, int** B, int m, int n, int p) {
//    int** C = new int* [m];
//    for (int i = 0; i < m; ++i) {
//        C[i] = new int[p];
//    }
//
//    // 计算矩阵C的每个元素  
//    for (int i = 0; i < m; ++i) {
//        for (int j = 0; j < p; ++j) {
//            C[i][j] = 0;
//            for (int k = 0; k < n; ++k) {
//                C[i][j] += A[i][k] * B[k][j];
//            }
//        }
//    }
//    return C;
//}
//
//// 函数用于打印矩阵  
//void print_matrix(int** matrix, int rows, int cols) {
//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            std::cout << matrix[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//}
//
//// 函数用于释放矩阵内存  
//void free_matrix(int** matrix, int rows) {
//    for (int i = 0; i < rows; ++i) {
//        delete[] matrix[i];
//    }
//    delete[] matrix;
//}
//
//int main() {
//    for (int i = 0; i < 10; i++)
//    {
//        int m;
//        std::cin >> m;
//        int n = m;
//        int p = m;
//
//        // 分配内存给矩阵A和B  
//        int** A = new int* [m];
//        int** B = new int* [n];
//        for (int i = 0; i < m; ++i) {
//            A[i] = new int[n];
//        }
//        for (int i = 0; i < n; ++i) {
//            B[i] = new int[p];
//        }
//
//        // 生成随机矩阵A和B  
//        std::srand(std::time(0));
//        for (int i = 0; i < m; ++i) {
//            for (int j = 0; j < n; ++j) {
//                A[i][j] = std::rand() % 21 - 10; // 产生-10到10之间的随机数  
//            }
//        }
//        for (int i = 0; i < n; ++i) {
//            for (int j = 0; j < p; ++j) {
//                B[i][j] = std::rand() % 21 - 10; // 产生-10到10之间的随机数  
//            }
//        }
//
//        // 计算开始时间  
//        auto start = std::chrono::high_resolution_clock::now();
//
//
//        // 执行矩阵乘法  
//        int** C = matrix_multiply(A, B, m, n, p);
//
//        // 计算结束时间  
//        clock_t end_time = clock();
//        auto end = std::chrono::high_resolution_clock::now();
//        std::chrono::duration<double> elapsed = end - start;
//        // 打印结果  
//        std::cout << "矩阵乘法完成" << std::endl;
//        std::cout << "计算耗时: " << elapsed.count() << " 秒" << std::endl;
//        //print_matrix(C, m, p);
//
//        // 释放矩阵内存  
//        free_matrix(A, m);
//        free_matrix(B, n);
//        free_matrix(C, m);
//    }
//    return 0;
//}