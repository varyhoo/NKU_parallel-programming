#include <iostream>
#include <windows.h>
using namespace std;
const int N = 4096;
int* a = new int [N];
int sum;
int sum1;
int sum2;
int loop2=100;
void init1()
{
	for (int i = 0; i < N; i++)
	{
		a[i] = i;
	}
}
void func_chain()
{
	long long int begin, end, freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&begin);
	for (int l = 0; l < loop2; l++)
	{
		for (int i = 0; i < N; i++)
		{
			sum += a[i];
		}
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&end);
	cout << "平凡算法用时:" << (end - begin) * 1000.0 / (freq*loop2) << "ms" << endl;
}
void func_superscalar() 
{
	long long int begin, end, freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&begin);
	for (int l = 0; l < loop2; l++)
	{
		for (int i = 0; i < N; i += 2)
		{
			sum1 += a[i];
			sum2 += a[i + 1];
		}
		sum = sum1 + sum2;
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&end);
	cout << "优化算法用时:" << (end - begin) * 1000.0 / (freq*loop2) << "ms" << endl;
}
 
 int main()
{
	init1();
	func_chain();
	func_superscalar();
	cout << "循环执行了" << loop2 << "次"<<endl;
	cout << "N为:" << N;
}
 
 
 
 