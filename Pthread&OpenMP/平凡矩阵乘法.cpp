//#include <iostream>  
//#include <ctime>  
//#include <cstdlib>  
//#include<chrono>
//
//// �������ھ���˷�  
//int** matrix_multiply(int** A, int** B, int m, int n, int p) {
//    int** C = new int* [m];
//    for (int i = 0; i < m; ++i) {
//        C[i] = new int[p];
//    }
//
//    // �������C��ÿ��Ԫ��  
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
//// �������ڴ�ӡ����  
//void print_matrix(int** matrix, int rows, int cols) {
//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            std::cout << matrix[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//}
//
//// ���������ͷž����ڴ�  
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
//        // �����ڴ������A��B  
//        int** A = new int* [m];
//        int** B = new int* [n];
//        for (int i = 0; i < m; ++i) {
//            A[i] = new int[n];
//        }
//        for (int i = 0; i < n; ++i) {
//            B[i] = new int[p];
//        }
//
//        // �����������A��B  
//        std::srand(std::time(0));
//        for (int i = 0; i < m; ++i) {
//            for (int j = 0; j < n; ++j) {
//                A[i][j] = std::rand() % 21 - 10; // ����-10��10֮��������  
//            }
//        }
//        for (int i = 0; i < n; ++i) {
//            for (int j = 0; j < p; ++j) {
//                B[i][j] = std::rand() % 21 - 10; // ����-10��10֮��������  
//            }
//        }
//
//        // ���㿪ʼʱ��  
//        auto start = std::chrono::high_resolution_clock::now();
//
//
//        // ִ�о���˷�  
//        int** C = matrix_multiply(A, B, m, n, p);
//
//        // �������ʱ��  
//        clock_t end_time = clock();
//        auto end = std::chrono::high_resolution_clock::now();
//        std::chrono::duration<double> elapsed = end - start;
//        // ��ӡ���  
//        std::cout << "����˷����" << std::endl;
//        std::cout << "�����ʱ: " << elapsed.count() << " ��" << std::endl;
//        //print_matrix(C, m, p);
//
//        // �ͷž����ڴ�  
//        free_matrix(A, m);
//        free_matrix(B, n);
//        free_matrix(C, m);
//    }
//    return 0;
//}