#define _CRT_SECURE_NO_WARNINGS
#ifndef _STDIO_H_ 
#define _STDIO_H_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include<iostream>
#include <math.h>
#include<time.h>
#include<chrono>
#define PI 3.1415926535898

//单条数据的长度
#define MAX_LINE 100
//数据集的长度
#define DATA_LEN 100000
//特征值的个数（身高、体重、肺活量）
#define EIGEN_NUM 4


/*声明函数*/
//获取数据集
void getData(const char* fileLocation);

//获得平均值和标准差：给 Parameter 赋值
void getParameter();
//计算平均值和标准差
double* calParameter(int column, int sex);

//计算概率p(特征列column = x | 性别)
double getProbability(double x, int column, int sex);
//正态分布 x:随机变量的值 u:样本平均值 p:标准差
double gaussianDistribution(double x, double u, double p);

//返回性别字符结果
char* sexResult(float height, float weight);
//返回性别ID结果
int sexIDResult(float height, float weight);
//准确度判断
float precision();

//数据集有肺活量(VC),返回性别字符结果
char* addVCSexResult(float height, float weight, float VC);
//数据集有肺活量(VC)，返回性别ID结果
int addVCSexIDResult(float height, float weight, float VC);
//数据集有肺活量(VC),准确度判断
float addVCPrecision();

#endif 
float dataSet[DATA_LEN][EIGEN_NUM];	//数据集
double maleNum = 0;//男性总数
double femaleNum = 0;//女性总数
double meanValue[2][3];			    //男性、女性的所有均值[0][0,1,2]=[男][身高、体重、肺活量]
double standardDeviation[2][3];     //男性、女性的所有标准差[0][0,1,2]=[男][身高、体重、肺活量]

int dataLen;				//从1开始计算的数据集的长度,数学计算用的长度
const char* basicInfo[] = { "性别","身高","体重","肺活量" };
//char *addVitalCapacityInfo[] = {"性别","身高","体重","肺活量"};

//读取数据
void getData(const char* fileLocation)
{
	char buf[MAX_LINE];		//缓冲区
	FILE* fp;				//文件指针s
	int len;				//行字符个数

	//printf("正在读取文件\n");

	//读取文件
	if ((fp = fopen(fileLocation, "r")) == NULL)
	{
		perror("fail to read");
		exit(1);
	}

	//逐行读取及写入数组
	char* token;
	const char s[2] = ",";
	int i = 0;
	int j = 0;
	while (fgets(buf, MAX_LINE, fp) != NULL && i < DATA_LEN - 1)
	{
		len = strlen(buf);
		buf[len - 1] = '\0';  //删去换行符
		//分割字符串
		token = strtok(buf, s);
		//继续分割字符串
		j = 0;
		while (token != NULL) {
			dataSet[i][j] = atof(token);
			token = strtok(NULL, s);
			j = j + 1;
		}
		i = i + 1;
	}
	dataLen = i;
	//计算男女个数
	for (i = 0; i < dataLen; i++) {
		if (dataSet[i][0] == 1) { maleNum = maleNum + 1; }
		if (dataSet[i][0] == 2) { femaleNum = femaleNum + 1; }
	}
	getParameter();
}

//获得平均值和标准差：给 Parameter 赋值
void getParameter()
{
	double* p;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < EIGEN_NUM - 1; j++) {
			p = calParameter(j + 1, i + 1);
			meanValue[i][j] = *p;
			standardDeviation[i][j] = *(p + 1);
		}
	}
}

//计算平均值和标准差
double* calParameter(int column, int sex)
{
	double  r[2];
	double sexNum;//性别人数
	double u = 0; //平均值
	double p = 0; //标准差
	double sum = 0;
	int i = 0;

	//判断性别
	char* gender=0;
	if (sex == 1)
	{
		sexNum = maleNum;
		gender = (char*)"男性";
	}
	if (sex == 2)
	{
		sexNum = femaleNum;
		gender = (char*)"女性";
	}

	//求平均值
	for (i = 0; i < dataLen; i++) {
		if (dataSet[i][0] == sex) {
			sum = sum + 1;
			u = u + dataSet[i][column];
			//printf("%lf\n",dataSet[i][column]);
		}
	}
	u = u / sum;

	//求标准差
	double numSum = 0;
	for (i = 0; i < dataLen; i++) {
		if (dataSet[i][0] == sex) {
			numSum = numSum + pow(dataSet[i][column] - u, 2);
		}
	}
	p = sqrt(numSum / sexNum);
	//printf("numSum=%f\n",numSum);
	printf("%s%s平均值=%.16lf\n标准差=%.16lf\n", gender, basicInfo[column], u, p);

	r[0] = u;
	r[1] = p;
	return r;
}


//计算概率p(特征列column = x | 性别)
double getProbability(double x, int column, int sex)
{
	double Probability;	//计算出的概率
	double u = meanValue[sex - 1][column - 1];
	double p = standardDeviation[sex - 1][column - 1];
	Probability = gaussianDistribution(x, u, p);

	//printf("p(%s=%lf|性别=%s)=%.16lf\n",basicInfo[column],x,gender,Probability);

	return Probability;
}


//正态分布 x:随机变量的值 u:样本平均值 p:标准差 y:概率
double gaussianDistribution(double x, double u, double p)
{
	double y;
	p = pow(p, 2);
	y = (1 / (2 * PI * p)) * exp(-pow((x - u), 2) / (2 * p));
	return y;
}

//返回性别字符结果
char* sexResult(float height, float weight)
{
	double maleP;//男性概率
	double femaleP;//女性概率
	double a = 0.5; //男女比例各50%

	maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1);
	//printf("\n");
	femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2);
	//printf("\n");

	if (maleP > femaleP) { return (char*)"男性"; }
	if (maleP < femaleP) { return(char*)"女性"; }
	if (maleP == femaleP) { return (char*)"未知"; }
	else return (char*)"未知";
}

//返回性别ID结果
int sexIDResult(float height, float weight)
{
	double maleP;//男性概率
	double femaleP;//女性概率
	double a = 0.5; //男女比例各50%

	maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1);
	//printf("\n");
	femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2);
	//printf("\n");

	if (maleP > femaleP) { return 1; }
	if (maleP < femaleP) { return 2; }
	if (maleP == femaleP) { return 0; }
	else return 999;
}

//准确度判断
float precision()
{
	int i;
	float preSexID;
	float right = 0;
	float error = 0;
	for (i = 0; i < dataLen; i++) {
		preSexID = sexIDResult(dataSet[i][1], dataSet[i][2]);
		//printf("预测ID:%f  实际ID:%f \n",preSexID,dataSet[i][0]);
		if (dataSet[i][0] == preSexID) { right = right + 1; }
		else { error = error + 1; }
	}
	printf("Right:%f\nError:%f\n", right, error);
	return right / (error + right);
}


//以下VC表肺活量
//数据集有肺活量(VC)，返回性别ID结果
char* addVCSexResult(float height, float weight, float VC)
{
	double maleP;//男性概率
	double femaleP;//女性概率
	double a = 0.5; //男女比例各50%

	maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1) * getProbability(VC, 3, 1);
	//printf("\n");
	femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2) * getProbability(VC, 3, 2);
	//printf("\n");

	if (maleP > femaleP) { return (char*)"男性"; }
	if (maleP < femaleP) { return (char*)"女性"; }
	if (maleP == femaleP) { return (char*)"未知"; }
	else return (char*)"未知";
}

//数据集有肺活量(VC)，返回性别ID结果
int addVCSexIDResult(float height, float weight, float VC)
{
	double maleP;//男性概率
	double femaleP;//女性概率
	double a = 0.5; //男女比例各50%

	maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1) * getProbability(VC, 3, 1);
	//printf("\n");
	femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2) * getProbability(VC, 3, 2);
	//printf("\n");

	if (maleP > femaleP) { return 1; }
	if (maleP < femaleP) { return 2; }
	if (maleP == femaleP) { return 0; }
	else return 999;
}

//数据集有肺活量(VC),准确度判断
float addVCPrecision()
{
	int i;
	float preSexID;
	float right = 0;
	float error = 0;
	for (i = 0; i < dataLen; i++) {
		preSexID = addVCSexIDResult(dataSet[i][1], dataSet[i][2], dataSet[i][3]);
		if (dataSet[i][0] == preSexID) { right = right + 1; }
		else {
			error = error + 1;
		}
	}
	printf("Right:%f\nError:%f\n", right, error);
	return right / (error + right);

}
int main() {
	//给全局变量dataSet赋值
	getData("addVitalCapacityData.csv");
	auto startTime = std::chrono::high_resolution_clock::now();
	printf("Accuracy：%f\n", addVCPrecision());
   auto endTime = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsedTime = endTime - startTime;
   std::cout<<std::endl << "Total convolution time: " << elapsedTime.count() << " seconds" ;
}