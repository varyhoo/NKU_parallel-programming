#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <chrono>
#include <immintrin.h>
#include <omp.h>
#include <mpi.h>

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
const char* basicInfo[] = { "Gender", "Height", "Weight", "VitalCapacity" };

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
        buf[len - 1] = '\0';  
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

double* calParameterAVX(int column, int sex) {
    double sum = 0;
    double u = 0;
    double numSum = 0;
    double r[2] __attribute__((aligned(32)));

    __m256d vecSum = _mm256_setzero_pd();
    __m256d vecNumSum = _mm256_setzero_pd();
    __m256d vecU = _mm256_setzero_pd();

    int i;
    for (i = 0; i < dataLen - 3; i += 4) {
        __m256d vecData = _mm256_loadu_pd(&dataSet[i][column]);
        __m256d vecSex = _mm256_loadu_pd(&dataSet[i][0]);
        __m256d vecMask = _mm256_cmp_pd(vecSex, _mm256_set1_pd(sex), _CMP_EQ_OQ);
        vecSum = _mm256_add_pd(vecSum, _mm256_and_pd(vecMask, _mm256_set1_pd(1.0)));
        vecU = _mm256_add_pd(vecU, _mm256_and_pd(vecMask, vecData));
    }

    sum = ((double*)&vecSum)[0] + ((double*)&vecSum)[1] + ((double*)&vecSum)[2] + ((double*)&vecSum)[3];
    u = ((double*)&vecU)[0] + ((double*)&vecU)[1] + ((double*)&vecU)[2] + ((double*)&vecU)[3];
    u /= sum;

    for (i = 0; i < dataLen - 3; i += 4) {
        __m256d vecData = _mm256_loadu_pd(&dataSet[i][column]);
        __m256d vecSex = _mm256_loadu_pd(&dataSet[i][0]);
        __m256d vecMask = _mm256_cmp_pd(vecSex, _mm256_set1_pd(sex), _CMP_EQ_OQ);
        __m256d vecDiff = _mm256_sub_pd(vecData, _mm256_set1_pd(u));
        vecNumSum = _mm256_add_pd(vecNumSum, _mm256_and_pd(vecMask, _mm256_mul_pd(vecDiff, vecDiff)));
    }

    numSum = ((double*)&vecNumSum)[0] + ((double*)&vecNumSum)[1] + ((double*)&vecNumSum)[2] + ((double*)&vecNumSum)[3];
    double p = sqrt(numSum / sum);

    r[0] = u;
    r[1] = p;
    return r;
}

void getParameterOMP() {
#pragma omp parallel for
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < EIGEN_NUM - 1; j++) {
            double* p = calParameterAVX(j + 1, i + 1);
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

void getDataMPI(const char* fileLocation, int rank, int size) {
    if (rank == 0) {
        getData(fileLocation);
    }
    MPI_Bcast(dataSet, DATA_LEN * EIGEN_NUM, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dataLen, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    getDataMPI("addVitalCapacityData.csv", rank, size);
    getParameterOMP();

    if (rank == 0) {
        int n;
        std::cin >> n;
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < n; i++) {
            float height = 178;
            float weight = 60;
            int predictedSex = sexIDResult(height, weight);
        }
        auto end = std::chrono::steady_clock::now();
        std::cout << "Algorithm completed" << std::endl;
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
