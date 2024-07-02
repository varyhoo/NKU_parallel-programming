#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <numeric>
#include <random>
#include <omp.h>
#include <mpi.h>
#include <immintrin.h>

using namespace std;

class GaussianNaiveBayes {
public:
    void fit(const vector<vector<double>>& X, const vector<int>& y) {
        int n_samples = X.size();
        int n_features = X[0].size();

        map<int, vector<vector<double>>> separated_by_class;
        for (int i = 0; i < n_samples; ++i) {
            separated_by_class[y[i]].push_back(X[i]);
        }

#pragma omp parallel for
        for (auto it = separated_by_class.begin(); it != separated_by_class.end(); ++it) {
            int class_label = it->first;
            vector<vector<double>>& features = it->second;
            vector<double> means(n_features);
            vector<double> variances(n_features);

            for (int j = 0; j < n_features; ++j) {
                __m256d sum_vec = _mm256_setzero_pd();
                for (size_t k = 0; k < features.size(); k += 4) {
                    __m256d vec = _mm256_loadu_pd(&features[k][j]);
                    sum_vec = _mm256_add_pd(sum_vec, vec);
                }
                double temp[4];
                _mm256_storeu_pd(temp, sum_vec);
                means[j] = (temp[0] + temp[1] + temp[2] + temp[3]) / features.size();
            }


            for (int j = 0; j < n_features; ++j) {
                __m256d sum_vec = _mm256_setzero_pd();
                for (size_t k = 0; k < features.size(); k += 4) {
                    __m256d vec = _mm256_loadu_pd(&features[k][j]);
                    __m256d mean_vec = _mm256_set1_pd(means[j]);
                    __m256d diff = _mm256_sub_pd(vec, mean_vec);
                    __m256d sq_diff = _mm256_mul_pd(diff, diff);
                    sum_vec = _mm256_add_pd(sum_vec, sq_diff);
                }
                double temp[4];
                _mm256_storeu_pd(temp, sum_vec);
                variances[j] = (temp[0] + temp[1] + temp[2] + temp[3]) / features.size();
                if (variances[j] == 0) {
                    variances[j] = 1e-6;
                }
            }

#pragma omp critical
            {
                class_statistics[class_label] = { means, variances };
                class_priors[class_label] = static_cast<double>(features.size()) / n_samples;
            }
        }
    }

    int predict(const vector<double>& x) const {
        double best_prob = -INFINITY;
        int best_class = -1;

#pragma omp parallel
        {
            double local_best_prob = -INFINITY;
            int local_best_class = -1;

#pragma omp for nowait
            for (size_t i = 0; i < class_statistics.size(); ++i) {
                auto it = class_statistics.begin();
                std::advance(it, i);
                int class_label = it->first;
                const auto& stats = it->second;
                double class_prob = log(class_priors.at(class_label));

                for (int j = 0; j < x.size(); ++j) {
                    class_prob += log(gaussian_probability(x[j], stats.means[j], stats.variances[j]));
                }

                if (class_prob > local_best_prob) {
                    local_best_prob = class_prob;
                    local_best_class = class_label;
                }
            }

#pragma omp critical
            {
                if (local_best_prob > best_prob) {
                    best_prob = local_best_prob;
                    best_class = local_best_class;
                }
            }
        }

        return best_class;
    }

private:
    struct ClassStatistics {
        vector<double> means;
        vector<double> variances;
    };

    map<int, ClassStatistics> class_statistics;
    map<int, double> class_priors;

    double gaussian_probability(double x, double mean, double variance) const {
        double exponent = exp(-pow(x - mean, 2) / (2 * variance));
        return (1 / sqrt(2 * M_PI * variance)) * exponent;
    }
};

void generate_random_data(vector<vector<double>>& X, vector<int>& y, int n_samples, int n_features, int n_classes) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 15);
    uniform_int_distribution<> class_dis(0, n_classes - 1);

    for (int i = 0; i < n_samples; ++i) {
        vector<double> sample(n_features);
        for (int j = 0; j < n_features; ++j) {
            sample[j] = dis(gen);
        }
        X.push_back(sample);
        y.push_back(class_dis(gen));
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int n_samples = 100; // 样本数量
    int n_features = 2;  // 特征数量
    int n_classes = 2;   // 类别数量

    vector<vector<double>> X;
    vector<int> y;

    if (world_rank == 0) {
        generate_random_data(X, y, n_samples, n_features, n_classes);
    }

    MPI_Bcast(&X[0][0], n_samples * n_features, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y[0], n_samples, MPI_INT, 0, MPI_COMM_WORLD);

    GaussianNaiveBayes gnb;
    gnb.fit(X, y);

    if (world_rank == 0) {
        vector<double> test_point = { 7.0, 8.0 }; // 测试点
        int predicted_class = gnb.predict(test_point);
        cout << "Predicted class for test point (7.0, 8.0): " << predicted_class << endl;
    }

    MPI_Finalize();
    return 0;
}
