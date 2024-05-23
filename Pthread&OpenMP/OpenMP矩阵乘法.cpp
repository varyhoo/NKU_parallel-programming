#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <omp.h>
using namespace std;
const int num_threads = 4;

void multiply_openmp(vector<vector<double>>& A, vector<vector<double>>& B, vector<vector<double>>& C, int n)
{
    omp_set_num_threads(num_threads);
#pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}


int main() {
    int m;
    cin >> m;
    int n = m;
    int p = n;

    vector<vector<double>> A(m, vector<double>(n));
    vector<vector<double>> B(n, vector<double>(p));
    vector<vector<double>> C(m, vector<double>(p, 0.0));

    // 随机数生成器
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-10, 10);

    // 生成随机矩阵 A
    for (auto& row : A) {
        for (auto& elem : row) {
            elem = dis(gen);
        }
    }
    // 生成随机矩阵 B
    for (auto& row : B) {
        for (auto& elem : row) {
            elem = dis(gen);
        }
    }

    auto start = chrono::steady_clock::now();

    // 使用OpenMP并行计算矩阵乘法
    multiply_openmp(A, B, C, n);

    auto end = chrono::steady_clock::now();
    cout << "矩阵乘法完成" << endl;
    chrono::duration<double> elapsed = end - start;
    cout << "计算耗时: " << elapsed.count() << " 秒" << endl;

    return 0;
}
