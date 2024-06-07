#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <mpi.h>
#include <cassert>
#include <cstring>
#include <cmath>

#define PI 3.1415926535898

//�������ݵĳ���
#define MAX_LINE 20
//���ݼ��ĳ���(��1��ʼ����)
#define DATA_LEN 11000000

#define EIGEN_NUM 4

//float dataSet[DATA_LEN * EIGEN_NUM];	//���ݼ�
float(*dataSet) = (float(*))malloc(sizeof(float) * DATA_LEN * EIGEN_NUM);

int dataLen;//���ݼ�������
double maleNum = 0;//��������
double femaleNum = 0;//Ů������

int main(int argc, char** argv) {


	int i = 0;
	int j = 0;

	int my_rank;       //��ǰ����id
	int comm_sz;       //���̵���Ŀ

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	double start, end, readTime;
	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	start = MPI_Wtime();

	/************************����0��ȡ�ļ�************************/
	if (my_rank == 0)
	{
		char buf[MAX_LINE];		//������
		FILE* fp;				//�ļ�ָ��s
		int len;				//���ַ�����

		//��ȡ�ļ�
		const char* fileLocation = "addVitalCapacityData.csv";
		fp = fopen(fileLocation, "r");
		if (fp == NULL)
		{
			perror("fp  == NULL");
			exit(1);
		}

		//���ж�ȡ��д������
		char* token;
		const char s[2] = ",";
		while (fgets(buf, MAX_LINE, fp) != NULL && i < DATA_LEN)
		{
			len = strlen(buf);
			//ɾȥ���з�
			buf[len - 1] = '\0';
			//�ָ��ַ���
			token = strtok(buf, s);
			//�����ָ��ַ���
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
		printf("%d��4�е����ݶ�ȡ���\n", dataLen);
		fclose(fp);

		//������Ů����
		for (i = 0; i < dataLen; i++) {
			if (dataSet[i * 4] == 1) { maleNum = maleNum + 1; }
			if (dataSet[i * 4] == 2) { femaleNum = femaleNum + 1; }
		}

		readTime = MPI_Wtime();
	}


	MPI_Bcast(&dataLen, 1, MPI_INT, 0, MPI_COMM_WORLD);


	/************************���м���************************/

	/***********�����˹�ֲ�***********/

		/*�������ɢ��ַ�����*/
	int* Sendcounts; //��ÿ�����̷ַ������ݳ���
	Sendcounts = (int*)malloc(comm_sz * sizeof(int));//�����ڴ�

	for (i = 0; i < comm_sz; i++)
	{
		if (i == comm_sz - 1) { Sendcounts[i] = (int)(dataLen / comm_sz + (dataLen % comm_sz)) * EIGEN_NUM; }
		else { Sendcounts[i] = (int)(dataLen / comm_sz) * EIGEN_NUM; }
		//printf("����%d�ַ��������ݳ��ȣ�%d\n",i,Sendcounts[i]);
	}

	int receiveDataNum; //���յ����ݳ���
	receiveDataNum = Sendcounts[my_rank];

	int* displs;	 //�����dataSet���ڴ�ƫ����
	displs = (int*)malloc(comm_sz * sizeof(int)); //�����ڴ�
	displs[0] = 0;
	for (i = 1; i < comm_sz; i++)
	{
		displs[i] = displs[i - 1] + Sendcounts[i - 1];
		//printf("displs[i]=%d",displs[i]);
		//printf("�ַ�������%d���ڴ�ƫ������%d\n",i,displs[i]);
	}


	//�������������յ�������
	float(*receiveBuf) = (float*)malloc((receiveDataNum) * sizeof(float));
	MPI_Scatterv(dataSet, Sendcounts, displs, MPI_FLOAT, receiveBuf, receiveDataNum, MPI_FLOAT, 0, MPI_COMM_WORLD);

	/****���****/
	const char* maenInf[6] = { "maleLength","maleWeight","maleVC","femaleLength","femaleWeight","femaleVC" };
	//������ͺ���
	double getSum(float* data, int datalen, int sex, int column);
	//������ߡ����ء��λ���
	double maleLength = getSum(receiveBuf, receiveDataNum, 1, 1);
	double maleWeight = getSum(receiveBuf, receiveDataNum, 1, 2);
	double maleVC = getSum(receiveBuf, receiveDataNum, 1, 3);
	//Ů����ߡ����ء��λ���
	double femaleLength = getSum(receiveBuf, receiveDataNum, 2, 1);
	double femaleWeight = getSum(receiveBuf, receiveDataNum, 2, 2);
	double femaleVC = getSum(receiveBuf, receiveDataNum, 2, 3);
	double sendSum[] = { maleLength,maleWeight,maleVC,femaleLength,femaleWeight,femaleVC };//ÿ��������������ĺ�
	double* reciveSum = (double*)malloc((6 * comm_sz) * sizeof(double)); //��������0������

	/****��ƽ��ֵ****/
	double mean[6] = { 0,0,0,0,0,0 };
	if (my_rank == 0) {
		MPI_Gather(sendSum, 6, MPI_DOUBLE, reciveSum, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		for (i = 0; i < comm_sz; i++) {
			for (j = 0; j < 6; j++) {
				mean[j] = mean[j] + reciveSum[i * 6 + j];
			}
		}
		for (i = 0; i < 6; i++) {
			if (i < 3) { mean[i] = mean[i] / maleNum; }
			if (i >= 3) { mean[i] = mean[i] / femaleNum; }
		}
		//��ӡƽ��ֵ�����ս��
		for (i = 0; i < 6; i++) {
			//printf("mean-%s=%.16f\n",maenInf[i],mean[i]);
			if (i == 5) { printf("\n"); }
		}
	}
	else {
		MPI_Gather(sendSum, 6, MPI_DOUBLE, reciveSum, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}

	//��ƽ��ֵ�㲥�����н���
	MPI_Bcast(&mean, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	/****���׼��****/
	double sendSigma[6] = { 0,0,0,0,0,0 }; //ÿ�������ϵľֲ��ۼ�
	double* reciveSigma = (double*)malloc((6 * comm_sz) * sizeof(double)); //��������0������
	//�������ۼӺ���
	double getSigma(float* data, int datalen, double mean, int sex, int column);
	i = 0;
	for (int s = 1; s <= 2; s++) {
		for (int j = 1; j <= 3; j++) {
			sendSigma[i] = getSigma(receiveBuf, receiveDataNum, mean[i], s, j);
			i = i + 1;
		}
	}

	double standardDeviation[6];	//��׼��
	if (my_rank == 0)
	{
		MPI_Gather(sendSigma, 6, MPI_DOUBLE, reciveSigma, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		double Sigma[6] = { 0,0,0,0,0,0 };	//�ۼ�
		for (i = 0; i < comm_sz; i++) {
			for (j = 0; j < 6; j++) {
				Sigma[j] = Sigma[j] + reciveSigma[i * 6 + j];
			}
		}
		double sexNum;
		for (i = 0; i < 6; i++) {
			if (i < 3)
			{
				sexNum = maleNum;
			}
			if (i >= 3)
			{
				sexNum = femaleNum;
			}
			standardDeviation[i] = sqrt(Sigma[i] / sexNum);
		}
	}
	else {
		MPI_Gather(sendSigma, 6, MPI_DOUBLE, reciveSigma, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}

	MPI_Bcast(&standardDeviation, 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);



	float preSexID;
	float right = 0;
	float error = 0;
	int sexIDResult(float height, float weight, float VC, double* mean, double* standardDeviation);
	for (int i = 0; i < receiveDataNum / EIGEN_NUM; i++) {
		preSexID = sexIDResult(receiveBuf[i * EIGEN_NUM + 1], receiveBuf[i * EIGEN_NUM + 2], receiveBuf[i * EIGEN_NUM + 3], mean, standardDeviation);
		if (receiveBuf[i * EIGEN_NUM] == preSexID) { right = right + 1; }
		else {
			error = error + 1;
		}
	}
	float sendRuslt[2] = { right,error };
	float* reciveRuslt = (float*)malloc((2 * comm_sz) * sizeof(float)); //��������0������
	if (my_rank == 0)
	{
		MPI_Gather(sendRuslt, 2, MPI_FLOAT, reciveRuslt, 2, MPI_FLOAT, 0, MPI_COMM_WORLD);

		float lastResult[2] = { 0,0 };
		float right;
		float error;
		for (i = 0; i < comm_sz; i++) {
			lastResult[0] = lastResult[0] + reciveRuslt[2 * i];
			lastResult[1] = lastResult[1] + reciveRuslt[2 * i + 1];
		}
		double accuracy = lastResult[0] / (lastResult[0] + lastResult[1]);
		printf("Accuracy��%f\n", accuracy);

	}
	else {
		MPI_Gather(sendRuslt, 2, MPI_FLOAT, reciveRuslt, 2, MPI_FLOAT, 0, MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	end = MPI_Wtime();

	MPI_Finalize();

	if (my_rank == 0) { 
		printf("Read data time = %f\n", readTime - start);
		printf("Calculate time = %f\n", end - readTime);
		printf("Run time = %f\n", end - start);
	}
}





//���
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

//��pow((data[i]-mean),2)���ۼ�
double getSigma(float* data, int recDatalen, double mean, int sex, int column) {
	double Sigma = 0;
	for (int i = 0; i < (recDatalen / EIGEN_NUM); i++) {
		if (data[i * EIGEN_NUM] == sex) {
			Sigma = Sigma + pow(data[i * EIGEN_NUM + column] - mean, 2);
		}
	}
	return Sigma;
}

/***********���ر�Ҷ˹����***********/

//�������p(������column = x | �Ա�)
double getProbability(double x, int column, int sex, double mean, double standardDeviation)
{
	double Probability;	//������ĸ���
	double u = mean;
	double p = standardDeviation;

	//�����ֲ������ܶȺ��� x:Ԥ����� u:����ƽ��ֵ p:��׼��
	p = pow(p, 2);
	Probability = (1 / (2 * PI * p)) * exp(-pow((x - u), 2) / (2 * p));
	return Probability;
}


//�����Ա�ID���
int sexIDResult(float height, float weight, float VC, double* mean, double* standardDeviation)
{
	double maleP;//���Ը���
	double femaleP;//Ů�Ը���
	double a = 0.5; //��Ů������50%

	maleP = a * getProbability(height, 1, 1, mean[0], standardDeviation[0]) * getProbability(weight, 2, 1, mean[1], standardDeviation[1])
		* getProbability(VC, 3, 1, mean[2], standardDeviation[2]);

	femaleP = a * getProbability(height, 1, 2, mean[3], standardDeviation[3]) * getProbability(weight, 2, 2, mean[4], standardDeviation[4])
		* getProbability(VC, 3, 2, mean[5], standardDeviation[5]);

	if (maleP > femaleP) { return 1; }
	if (maleP < femaleP) { return 2; }
	if (maleP == femaleP) { return 0; }
}