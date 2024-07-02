#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <numeric>
#include <random>
const int M_PI = 3.14159265;
using namespace std;

class GaussianNaiveBayes {
public:
    void fit(const vector<vector<double>>& X, const vector<int>& y) {
        int n_samples = X.size();
        int n_features = X[0].size();

        // 按照不同的类别，对数据进行划分
        map<int, vector<vector<double>>> separated_by_class;
        for (int i = 0; i < n_samples; ++i) {
            separated_by_class[y[i]].push_back(X[i]);
        }

        // 计算平均值和（协）方差
        for (auto& class_pair : separated_by_class) {
            int class_label = class_pair.first;
            vector<vector<double>>& features = class_pair.second;
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
            }

            class_statistics[class_label] = { means, variances };
            class_priors[class_label] = static_cast<double>(features.size()) / n_samples;
        }
    }

    int predict(const vector<double>& x) const {
        double best_prob = -1.0;
        int best_class = -1;

        for (const auto& class_pair : class_statistics) {
            int class_label = class_pair.first;
            const auto& stats = class_pair.second;
            double class_prob = log(class_priors.at(class_label));

            for (int j = 0; j < x.size(); ++j) {
                class_prob += log(gaussian_probability(x[j], stats.means[j], stats.variances[j]));
            }

            if (class_prob > best_prob) {
                best_prob = class_prob;
                best_class = class_label;
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
    int n_samples, n_features, n_classes;

    cout << "Enter the number of samples: ";
    cin >> n_samples;

    cout << "Enter the number of features: ";
    cin >> n_features;

    cout << "Enter the number of classes: ";
    cin >> n_classes;

    vector<vector<double>> X;
    vector<int> y;

    generate_random_data(X, y, n_samples, n_features, n_classes);

    GaussianNaiveBayes gnb;
    gnb.fit(X, y);

    vector<double> test_point(n_features);
    cout << "Enter the test point values:" << endl;
    for (int i = 0; i < n_features; ++i) {
        cout << "Feature " << i + 1 << ": ";
        cin >> test_point[i];
    }

    int predicted_class = gnb.predict(test_point);

    cout << "Predicted class: " << predicted_class << endl;

    return 0;
}

