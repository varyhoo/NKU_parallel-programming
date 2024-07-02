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

//�������ݵĳ���
#define MAX_LINE 100
//���ݼ��ĳ���
#define DATA_LEN 100000
//����ֵ�ĸ�������ߡ����ء��λ�����
#define EIGEN_NUM 4


/*��������*/
//��ȡ���ݼ�
void getData(const char* fileLocation);

//���ƽ��ֵ�ͱ�׼��� Parameter ��ֵ
void getParameter();
//����ƽ��ֵ�ͱ�׼��
double* calParameter(int column, int sex);

//�������p(������column = x | �Ա�)
double getProbability(double x, int column, int sex);
//��̬�ֲ� x:���������ֵ u:����ƽ��ֵ p:��׼��
double gaussianDistribution(double x, double u, double p);

//�����Ա��ַ����
char* sexResult(float height, float weight);
//�����Ա�ID���
int sexIDResult(float height, float weight);
//׼ȷ���ж�
float precision();

//���ݼ��зλ���(VC),�����Ա��ַ����
char* addVCSexResult(float height, float weight, float VC);
//���ݼ��зλ���(VC)�������Ա�ID���
int addVCSexIDResult(float height, float weight, float VC);
//���ݼ��зλ���(VC),׼ȷ���ж�
float addVCPrecision();

#endif 
float dataSet[DATA_LEN][EIGEN_NUM];	//���ݼ�
double maleNum = 0;//��������
double femaleNum = 0;//Ů������
double meanValue[2][3];			    //���ԡ�Ů�Ե����о�ֵ[0][0,1,2]=[��][��ߡ����ء��λ���]
double standardDeviation[2][3];     //���ԡ�Ů�Ե����б�׼��[0][0,1,2]=[��][��ߡ����ء��λ���]

int dataLen;				//��1��ʼ��������ݼ��ĳ���,��ѧ�����õĳ���
const char* basicInfo[] = { "�Ա�","���","����","�λ���" };
//char *addVitalCapacityInfo[] = {"�Ա�","���","����","�λ���"};

//��ȡ����
void getData(const char* fileLocation)
{
	char buf[MAX_LINE];		//������
	FILE* fp;				//�ļ�ָ��s
	int len;				//���ַ�����

	//printf("���ڶ�ȡ�ļ�\n");

	//��ȡ�ļ�
	if ((fp = fopen(fileLocation, "r")) == NULL)
	{
		perror("fail to read");
		exit(1);
	}

	//���ж�ȡ��д������
	char* token;
	const char s[2] = ",";
	int i = 0;
	int j = 0;
	while (fgets(buf, MAX_LINE, fp) != NULL && i < DATA_LEN - 1)
	{
		len = strlen(buf);
		buf[len - 1] = '\0';  //ɾȥ���з�
		//�ָ��ַ���
		token = strtok(buf, s);
		//�����ָ��ַ���
		j = 0;
		while (token != NULL) {
			dataSet[i][j] = atof(token);
			token = strtok(NULL, s);
			j = j + 1;
		}
		i = i + 1;
	}
	dataLen = i;
	//������Ů����
	for (i = 0; i < dataLen; i++) {
		if (dataSet[i][0] == 1) { maleNum = maleNum + 1; }
		if (dataSet[i][0] == 2) { femaleNum = femaleNum + 1; }
	}
	getParameter();
}

//���ƽ��ֵ�ͱ�׼��� Parameter ��ֵ
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

//����ƽ��ֵ�ͱ�׼��
double* calParameter(int column, int sex)
{
	double  r[2];
	double sexNum;//�Ա�����
	double u = 0; //ƽ��ֵ
	double p = 0; //��׼��
	double sum = 0;
	int i = 0;

	//�ж��Ա�
	char* gender=0;
	if (sex == 1)
	{
		sexNum = maleNum;
		gender = (char*)"����";
	}
	if (sex == 2)
	{
		sexNum = femaleNum;
		gender = (char*)"Ů��";
	}

	//��ƽ��ֵ
	for (i = 0; i < dataLen; i++) {
		if (dataSet[i][0] == sex) {
			sum = sum + 1;
			u = u + dataSet[i][column];
			//printf("%lf\n",dataSet[i][column]);
		}
	}
	u = u / sum;

	//���׼��
	double numSum = 0;
	for (i = 0; i < dataLen; i++) {
		if (dataSet[i][0] == sex) {
			numSum = numSum + pow(dataSet[i][column] - u, 2);
		}
	}
	p = sqrt(numSum / sexNum);
	//printf("numSum=%f\n",numSum);
	printf("%s%sƽ��ֵ=%.16lf\n��׼��=%.16lf\n", gender, basicInfo[column], u, p);

	r[0] = u;
	r[1] = p;
	return r;
}


//�������p(������column = x | �Ա�)
double getProbability(double x, int column, int sex)
{
	double Probability;	//������ĸ���
	double u = meanValue[sex - 1][column - 1];
	double p = standardDeviation[sex - 1][column - 1];
	Probability = gaussianDistribution(x, u, p);

	//printf("p(%s=%lf|�Ա�=%s)=%.16lf\n",basicInfo[column],x,gender,Probability);

	return Probability;
}


//��̬�ֲ� x:���������ֵ u:����ƽ��ֵ p:��׼�� y:����
double gaussianDistribution(double x, double u, double p)
{
	double y;
	p = pow(p, 2);
	y = (1 / (2 * PI * p)) * exp(-pow((x - u), 2) / (2 * p));
	return y;
}

//�����Ա��ַ����
char* sexResult(float height, float weight)
{
	double maleP;//���Ը���
	double femaleP;//Ů�Ը���
	double a = 0.5; //��Ů������50%

	maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1);
	//printf("\n");
	femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2);
	//printf("\n");

	if (maleP > femaleP) { return (char*)"����"; }
	if (maleP < femaleP) { return(char*)"Ů��"; }
	if (maleP == femaleP) { return (char*)"δ֪"; }
	else return (char*)"δ֪";
}

//�����Ա�ID���
int sexIDResult(float height, float weight)
{
	double maleP;//���Ը���
	double femaleP;//Ů�Ը���
	double a = 0.5; //��Ů������50%

	maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1);
	//printf("\n");
	femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2);
	//printf("\n");

	if (maleP > femaleP) { return 1; }
	if (maleP < femaleP) { return 2; }
	if (maleP == femaleP) { return 0; }
	else return 999;
}

//׼ȷ���ж�
float precision()
{
	int i;
	float preSexID;
	float right = 0;
	float error = 0;
	for (i = 0; i < dataLen; i++) {
		preSexID = sexIDResult(dataSet[i][1], dataSet[i][2]);
		//printf("Ԥ��ID:%f  ʵ��ID:%f \n",preSexID,dataSet[i][0]);
		if (dataSet[i][0] == preSexID) { right = right + 1; }
		else { error = error + 1; }
	}
	printf("Right:%f\nError:%f\n", right, error);
	return right / (error + right);
}


//����VC��λ���
//���ݼ��зλ���(VC)�������Ա�ID���
char* addVCSexResult(float height, float weight, float VC)
{
	double maleP;//���Ը���
	double femaleP;//Ů�Ը���
	double a = 0.5; //��Ů������50%

	maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1) * getProbability(VC, 3, 1);
	//printf("\n");
	femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2) * getProbability(VC, 3, 2);
	//printf("\n");

	if (maleP > femaleP) { return (char*)"����"; }
	if (maleP < femaleP) { return (char*)"Ů��"; }
	if (maleP == femaleP) { return (char*)"δ֪"; }
	else return (char*)"δ֪";
}

//���ݼ��зλ���(VC)�������Ա�ID���
int addVCSexIDResult(float height, float weight, float VC)
{
	double maleP;//���Ը���
	double femaleP;//Ů�Ը���
	double a = 0.5; //��Ů������50%

	maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1) * getProbability(VC, 3, 1);
	//printf("\n");
	femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2) * getProbability(VC, 3, 2);
	//printf("\n");

	if (maleP > femaleP) { return 1; }
	if (maleP < femaleP) { return 2; }
	if (maleP == femaleP) { return 0; }
	else return 999;
}

//���ݼ��зλ���(VC),׼ȷ���ж�
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
	//��ȫ�ֱ���dataSet��ֵ
	getData("addVitalCapacityData.csv");
	auto startTime = std::chrono::high_resolution_clock::now();
	printf("Accuracy��%f\n", addVCPrecision());
   auto endTime = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsedTime = endTime - startTime;
   std::cout<<std::endl << "Total convolution time: " << elapsedTime.count() << " seconds" ;
}