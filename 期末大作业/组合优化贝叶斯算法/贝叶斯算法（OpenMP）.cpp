#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <cassert>
#include <cstring>
#include <cmath>
#include<omp.h>


#define PI 3.1415926535898

//单条数据的长度
#define MAX_LINE 20
//数据集的长度(从1开始计算)
#define DATA_LEN 5000000

#define EIGEN_NUM 4

float(*dataSet) = (float(*))malloc(sizeof(float) * DATA_LEN * EIGEN_NUM);

int dataLen;//数据集的行数
double maleNum = 0;//男性总数
double femaleNum = 0;//女性总数

double getSum(float* data, int recDatalen, int sex, int column)
{
	double Sum = 0;
	for (int i = 0; i < (recDatalen / EIGEN_NUM); i++)
	{
		if (data[i * EIGEN_NUM] == sex) {
			Sum = Sum + data[i * EIGEN_NUM + column];
		}
	}
	return Sum;
}

//求pow((data[i]-mean),2)的累加
double getSigma(float* data, int recDatalen, double mean, int sex, int column) {
	double Sigma = 0;
	for (int i = 0; i < (recDatalen / EIGEN_NUM); i++) {
		if (data[i * EIGEN_NUM] == sex) {
			Sigma = Sigma + pow(data[i * EIGEN_NUM + column] - mean, 2);
		}
	}
	return Sigma;
}

//计算概率p(特征列column = x | 性别)
double getProbability(double x, int column, int sex, double mean, double standardDeviation)
{
	double Probability;	//计算出的概率
	double u = mean;
	double p = standardDeviation;

	//高数分布概率密度函数 x:预测变量 u:样本平均值 p:标准差
	p = pow(p, 2);
	Probability = (1 / (2 * PI * p)) * exp(-pow((x - u), 2) / (2 * p));

	return Probability;
}

//返回性别ID结果
int sexIDResult(float height, float weight, float VC, double* mean, double* standardDeviation)
{
	double maleP;//男性概率
	double femaleP;//女性概率
	double a = 0.5; //男女比例各50%

	maleP = a * getProbability(height, 1, 1, mean[0], standardDeviation[0]) * getProbability(weight, 2, 1, mean[1], standardDeviation[1])
		* getProbability(VC, 3, 1, mean[2], standardDeviation[2]);

	femaleP = a * getProbability(height, 1, 2, mean[3], standardDeviation[3]) * getProbability(weight, 2, 2, mean[4], standardDeviation[4])
		* getProbability(VC, 3, 2, mean[5], standardDeviation[5]);

	if (maleP > femaleP) { return 1; }
	if (maleP < femaleP) { return 2; }
	if (maleP == femaleP) { return 0; }
}


int main(int argc, char** argv) {

	int i = 0;
	int j = 0;

	double start = omp_get_wtime();



	char buf[MAX_LINE];		//缓冲区
	FILE* fp;				//文件指针s
	int len;				//行字符个数

	//读取文件
	const char* fileLocation = "addVitalCapacityData1.csv";
	fp = fopen(fileLocation, "r");
	if (fp == NULL)
	{
		perror("fp  == NULL");
		exit(1);
	}

	//逐行读取及写入数组
	char* token;
	const char s[2] = ",";
	while (fgets(buf, MAX_LINE, fp) != NULL && i < DATA_LEN)
	{
		len = strlen(buf);
		//删去换行符
		buf[len - 1] = '\0';
		//分割字符串
		token = strtok(buf, s);
		//继续分割字符串
		j = 0;
		while (token != NULL)
		{
			dataSet[i * EIGEN_NUM + j] = atof(token);
			token = strtok(NULL, s);
			j = j + 1;
		}
		i = i + 1;
	}
	dataLen = i;
	printf("%d行4列的数据读取完毕\n", dataLen);
	fclose(fp);



	double readTime = omp_get_wtime();



	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <thread_count>" << std::endl;
		return 1;
	}

	char* endptr;
	int thread_count = strtol(argv[1], &endptr, 10);
	if (*endptr != '\0') {
		std::cerr << "Invalid thread count: " << argv[1] << std::endl;
		return 1;
	}


	/***********计算高斯分布***********/
	const char* maenInf[6] = { "maleLength","maleWeight","maleVC","femaleLength","femaleWeight","femaleVC" };

	double A, B, C, D, E, F, G;
	A = 0; B = 0; C = 0; D = 0; E = 0; F = 0; G = 0;

	double sum[6] = { 0,0,0,0,0,0 };
	double mean[6] = { 0,0,0,0,0,0 };


#	pragma omp parallel for num_threads(thread_count) \
	reduction(+:maleNum) reduction(+:femaleNum) \
	reduction(+:A) reduction(+:B) reduction(+:C)\
	reduction(+:D) reduction(+:E) reduction(+:F)\
	shared(dataSet,dataLen) private(i)
	for (i = 0; i < dataLen; i++)
	{
		if (dataSet[i * EIGEN_NUM] == 1)
		{
			maleNum = maleNum + 1;
			A += dataSet[i * EIGEN_NUM + 1];
			B += dataSet[i * EIGEN_NUM + 2];
			C += dataSet[i * EIGEN_NUM + 3];
		}
		else if (dataSet[i * EIGEN_NUM] == 2)
		{
			femaleNum = femaleNum + 1;
			D += dataSet[i * EIGEN_NUM + 1];
			E += dataSet[i * EIGEN_NUM + 2];
			F += dataSet[i * EIGEN_NUM + 3];
		}
		else
		{
			printf("dataSet[%d]=%f,性别有误\n", i * EIGEN_NUM, dataSet[i * EIGEN_NUM]);
		}
	}
#	pragma omp barrier
	sum[0] = A;
	sum[1] = B;
	sum[2] = C;
	sum[3] = D;
	sum[4] = E;
	sum[5] = F;
	//计算平均值
	for (i = 0; i < 6; i++)
	{
		if (i < 3) { mean[i] = sum[i] / maleNum; }
		if (i > 2) { mean[i] = sum[i] / femaleNum; }
	}

	//计算累加
	A = 0; B = 0; C = 0; D = 0; E = 0; F = 0; G = 0;
	double Sigma[6] = { 0,0,0,0,0,0 };
#	pragma omp parallel for num_threads(thread_count) default(none) \
	reduction(+:A) reduction(+:B) reduction(+:C)\
	reduction(+:D) reduction(+:E) reduction(+:F)\
	shared(dataSet,dataLen,mean) private(i)
	for (i = 0; i < dataLen; i++)
	{
		if (dataSet[i * EIGEN_NUM] == 1)
		{
			A += pow(dataSet[i * EIGEN_NUM + 1] - mean[0], 2);
			B += pow(dataSet[i * EIGEN_NUM + 2] - mean[1], 2);
			C += pow(dataSet[i * EIGEN_NUM + 3] - mean[2], 2);
		}
		else if (dataSet[i * EIGEN_NUM] == 2)
		{
			D += pow(dataSet[i * EIGEN_NUM + 1] - mean[3], 2);
			E += pow(dataSet[i * EIGEN_NUM + 2] - mean[4], 2);
			F += pow(dataSet[i * EIGEN_NUM + 3] - mean[5], 2);
		}
		else
		{
			std::cout<<"有误"<<dataSet[i * EIGEN_NUM];
		}
	}
#	pragma omp barrier
	Sigma[0] = A;
	Sigma[1] = B;
	Sigma[2] = C;
	Sigma[3] = D;
	Sigma[4] = E;
	Sigma[5] = F;


	//计算标准差
	double standardDeviation[6];	//标准差
	double sexNum;//各性别人数
	for (i = 0; i < 6; i++) {
		if (i < 3) { sexNum = maleNum; }
		if (i >= 3) { sexNum = femaleNum; }
		standardDeviation[i] = sqrt(Sigma[i] / sexNum);
		//printf("Sigma[%d]=%f maleNum=%f",i,Sigma[i],sexNum);
		//printf("第%d个标准差=%.5f\n",i,standardDeviation[i]);
	}



	float preSexID;
	float Right = 0;
	float Error = 0;
	int sexIDResult(float height, float weight, float VC, double* mean, double* standardDeviation);

#	pragma omp parallel for num_threads(thread_count)  default(none) \
	reduction(+:Right) reduction(+:Error) \
	shared(dataSet,dataLen,mean,standardDeviation) private(i,preSexID)
	for (i = 0; i < dataLen; i++) {
		preSexID = sexIDResult(dataSet[i * EIGEN_NUM + 1], dataSet[i * EIGEN_NUM + 2], dataSet[i * EIGEN_NUM + 3], mean, standardDeviation);
		if (dataSet[i * EIGEN_NUM] == preSexID) {
			Right = Right + 1;
		}
		else {
			Error = Error + 1;
		}
	}

	std::cout << "Right:" << Right << "Error" << Error;
	double accuracy = Right / (Error + Right);
	std::cout << "Accuracy：" << accuracy;
	double end = omp_get_wtime();
	printf("整体耗时 = %.16f\n", end - start);
	std::cout << end - start << std::endl << readTime - start << std::endl << end - readTime;
	return 0;
}