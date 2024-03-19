#include <iostream>
#include <windows.h>
using namespace std;
const int N = 1000;
int a[N];
int b[N][N];
int sum[N];
long int loop1 = 10;
void init()
{
    for (int i = 0; i < N; i++)
    {
        a[i] = i;
        
    }
       
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            b[i][j] = i * j;
}

void func_01()//矩阵向量积的平凡算法
{
    long long int begin, end, freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&begin);
    for (int l = 0; l < loop1; l++)
    {
        for (int i = 0; i < N; i++)
        {
            sum[i] = 0;
            for (int j = 0; j < N; j++)
            {
                sum[i] += a[j] * b[j][i];
            }
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&end);
    cout << "平凡算法用时:" << (end - begin) * 1000.0 / (freq*loop1) << "ms" << endl;
}

void func_02()//矩阵向量积的cache优化算法
{
    long long int begin, end, freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&begin);
    for (int l = 0; l < loop1; l++)
    {
        for (int i = 0; i < N; i++)
        {
            sum[i] = 0;
        }
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                sum[j] += b[i][j] * a[i];
            }
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&end);
    cout << "优化算法用时:" << (end - begin) * 1000.0 / (freq * loop1) << "ms" << endl;
}

void func_03()
{
    long long int begin, end, freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&begin);
    for (int l = 0; l < loop1; l++)
    {
        for (int i = 0; i < N; i++)
        {
            sum[i] = 0;
        }
        for (int i = 0; i < N; i += 10)
        {
            int t0 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0, t6 = 0, t7 = 0, t8 = 0, t9 = 0;
            for (int j = 0; j < N; j++)
            {
                t0 = a[i] * b[i][j];
                t1 = a[i + 1] * b[i + 1][j];
                t2 = a[i + 2] * b[i + 2][j];
                t3 = a[i + 3] * b[i + 3][j];
                t4 = a[i + 4] * b[i + 4][j];
                t5 = a[i + 5] * b[i + 5][j];
                t6 = a[i + 6] * b[i + 6][j];
                t7 = a[i + 7] * b[i + 7][j];
                t8 = a[i + 8] * b[i + 8][j];
                t9 = a[i + 9] * b[i + 9][j];
            }
            sum[i] = t0;
            sum[i + 1] = t1;
            sum[i + 2] = t2;
            sum[i + 3] = t3;
            sum[i + 4] = t4;
            sum[i + 5] = t5;
            sum[i + 6] = t6;
            sum[i + 7] = t7;
            sum[i + 8] = t8;
            sum[i + 9] = t9;
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&end);
    cout << "unroll算法用时:" << (end - begin) * 1000.0 / (freq * loop1) << "ms" << endl;
}

/*

int main()
{
   init();
    func_01();
    func_02();
    func_03();
    cout << "循环执行了" << loop1 << "次"<<endl;
    cout << "N为" << N;
}


*/



