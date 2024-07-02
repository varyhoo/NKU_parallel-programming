#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <numeric>
#include <random>
#include <omp.h>
#include <chrono>
using namespace std;
const int M_PI = 3.14159265;
class GaussianNaiveBayes {
public:
    void fit(const vector<vector<double>>& X, const vector<int>& y) {
        int n_samples = X.size();
        int n_features = X[0].size();

        // Separate the data by class
        map<int, vector<vector<double>>> separated_by_class;
        for (int i = 0; i < n_samples; ++i) {
            separated_by_class[y[i]].push_back(X[i]);
        }

        // Calculate the mean and variance for each class and feature
#pragma omp parallel for
        for (auto it = separated_by_class.begin(); it != separated_by_class.end(); ++it) {
            int class_label = it->first;
            vector<vector<double>>& features = it->second;
            vector<double> means(n_features);
            vector<double> variances(n_features);

            for (int j = 0; j < n_features; ++j) {
                double sum = 0.0;
                for (auto& feature : features) {
                    sum += feature[j];
                }
                means[j] = sum / features.size();
            }

            for (int j = 0; j < n_features; ++j) {
                double variance_sum = 0.0;
                for (auto& feature : features) {
                    variance_sum += pow(feature[j] - means[j], 2);
                }
                variances[j] = variance_sum / features.size();
                // Avoid zero variance
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

int main() {
    int n_samples = 100000; // 样本数量
    int n_features = 8;  // 特征数量
    int n_classes = 8;   // 类别数量

    vector<vector<double>> X;
    vector<int> y;

    generate_random_data(X, y, n_samples, n_features, n_classes);
    auto startTime = std::chrono::high_resolution_clock::now();
    GaussianNaiveBayes gnb;
    gnb.fit(X, y);
           // 卷积操作计时结束
            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedTime = endTime - startTime;
            // 输出卷积操作的总时间
            std::cout << "Total convolution time: " << elapsedTime.count() << " seconds" << std::endl;
    // 输出训练数据的一部分以供验证
    cout << "Sample training data:" << endl;
    for (int i = 0; i < 5; ++i) {
        cout << "Sample " << i + 1 << ": ";
        for (double feature : X[i]) {
            cout << feature << " ";
        }
        cout << "Label: " << y[i] << endl;
    }

    vector<double> test_point = { 7.0, 8.0 }; // 测试点
    int predicted_class = gnb.predict(test_point);

    cout << "Predicted class for test point (7.0, 8.0): " << predicted_class << endl;

    return 0;
}
