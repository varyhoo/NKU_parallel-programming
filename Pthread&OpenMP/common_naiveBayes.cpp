#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include<iostream>
#include<chrono>

#define MAX_LINE 1024
#define DATA_LEN 1000
#define EIGEN_NUM 4
#define PI 3.1415926535898

float dataSet[DATA_LEN][EIGEN_NUM];
int dataLen;
double maleNum = 0;
double femaleNum = 0;
double meanValue[2][3];
double standardDeviation[2][3];
const char* basicInfo[] = { "性别", "身高", "体重", "肺活量" };

void getData(const char* fileLocation) {
    FILE* fp;
    char buf[MAX_LINE];
    int len;

    if ((fp = fopen(fileLocation, "r")) == NULL) {
        perror("fail to read");
        exit(1);
    }

    int i = 0;
    while (fgets(buf, MAX_LINE, fp) != NULL && i < DATA_LEN) {
        len = strlen(buf);
        buf[len - 1] = '\0';  // Remove newline character
        char* token = strtok(buf, ",");
        int j = 0;
        while (token != NULL && j < EIGEN_NUM) {
            dataSet[i][j] = atof(token);
            token = strtok(NULL, ",");
            j++;
        }
        i++;
    }
    dataLen = i;

    for (i = 0; i < dataLen; i++) {
        if (dataSet[i][0] == 1) {
            maleNum++;
        }
        else if (dataSet[i][0] == 2) {
            femaleNum++;
        }
    }
    fclose(fp);
}

double* calParameter(int column, int sex) {
    double sum = 0;
    double numSum = 0;
    double sexNum;
    double u = 0;
    double p = 0;
    double r[2];

    for (int i = 0; i < dataLen; i++) {
        if (dataSet[i][0] == sex) {
            sum++;
            u += dataSet[i][column];
        }
    }
    u /= sum;

    for (int i = 0; i < dataLen; i++) {
        if (dataSet[i][0] == sex) {
            numSum += pow(dataSet[i][column] - u, 2);
        }
    }
    p = sqrt(numSum / sum);

    r[0] = u;
    r[1] = p;
    return r;
}

void getParameter() {
    double* p;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < EIGEN_NUM - 1; j++) {
            p = calParameter(j + 1, i + 1);
            meanValue[i][j] = *p;
            standardDeviation[i][j] = *(p + 1);
        }
    }
}

double gaussianDistribution(double x, double u, double p) {
    p = pow(p, 2);
    double y = (1 / (sqrt(2 * PI) * p)) * exp(-pow((x - u), 2) / (2 * p));
    return y;
}

double getProbability(double x, int column, int sex) {
    double u = meanValue[sex - 1][column - 1];
    double p = standardDeviation[sex - 1][column - 1];
    return gaussianDistribution(x, u, p);
}

int sexIDResult(float height, float weight) {
    double maleP = 0;
    double femaleP = 0;
    double a = 0.5; // Male and female ratio

    maleP = a * getProbability(height, 1, 1) * getProbability(weight, 2, 1);
    femaleP = a * getProbability(height, 1, 2) * getProbability(weight, 2, 2);

    if (maleP > femaleP) {
        return 1;
    }
    else if (maleP < femaleP) {
        return 2;
    }
    else {
        return 0;
    }
}

int main() {

    getData("addVitalCapacityData.csv");
    getParameter();
    int n;
    std::cin >> n;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < n; i++)
    {
        float height = 178;
        float weight = 60;
        int predictedSex = sexIDResult(height, weight);
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "算法完成" << std::endl;
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "计算耗时: " << elapsed.count() << " 秒" << std::endl;

    return 0;
}
