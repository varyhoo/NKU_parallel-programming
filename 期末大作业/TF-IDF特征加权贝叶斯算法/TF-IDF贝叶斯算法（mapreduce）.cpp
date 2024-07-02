#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <random>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

// 分词函数，将文本拆分为单词
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    stringstream ss(text);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// 计算词频
map<string, int> computeWordFrequencies(const vector<string>& tokens) {
    map<string, int> wordFreq;
    for (const string& word : tokens) {
        wordFreq[word]++;
    }
    return wordFreq;
}

// 计算TF-IDF权重
map<string, double> computeTFIDF(const vector<string>& tokens, const map<string, int>& docFreq, int numDocs) {
    map<string, int> termFreq = computeWordFrequencies(tokens);
    map<string, double> tfidf;
    for (const auto& pair : termFreq) {
        string term = pair.first;
        double tf = static_cast<double>(pair.second) / tokens.size();
        double idf = log(static_cast<double>(numDocs) / (1 + (docFreq.count(term) > 0 ? docFreq.at(term) : 0)));
        tfidf[term] = tf * idf;
    }
    return tfidf;
}

class NaiveBayesClassifier {
private:
    map<string, map<string, double>> wordProb; // 每个类别中每个单词的条件概率
    map<string, double> classProb; // 每个类别的先验概率
    map<string, int> docFreq; // 每个单词在多少个文档中出现过
    int numDocs;

public:
    NaiveBayesClassifier() : numDocs(0) {}

    void train(const map<string, vector<vector<string>>>& trainingData) {
        map<string, int> classDocCount;

        for (const auto& pair : trainingData) {
            string className = pair.first;
            const vector<vector<string>>& docs = pair.second;
            classDocCount[className] = docs.size();
            numDocs += docs.size();

            for (const auto& doc : docs) {
                for (const auto& term : doc) {
                    docFreq[term]++;
                }
            }
        }

        // 使用MapReduce计算TF-IDF
        map<string, map<string, double>> intermediate;
        vector<thread> threads;
        mutex mtx;
        condition_variable cv;
        queue<vector<string>> docQueue;
        for (const auto& pair : trainingData) {
            for (const auto& doc : pair.second) {
                docQueue.push(doc);
            }
        }

        auto worker = [&]() {
            while (true) {
                vector<string> doc;
                {
                    unique_lock<mutex> lock(mtx);
                    if (docQueue.empty()) break;
                    doc = docQueue.front();
                    docQueue.pop();
                }
                map<string, double> tfidf = computeTFIDF(doc, docFreq, numDocs);
                {
                    unique_lock<mutex> lock(mtx);
                    for (const auto& pair : tfidf) {
                        intermediate[pair.first][doc[0]] += pair.second; // 将每个文档首单词作为文档ID
                    }
                }
            }
            };

        for (int i = 0; i < 4; ++i) { // 假设使用4个线程
            threads.emplace_back(worker);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        for (const auto& pair : trainingData) {
            string className = pair.first;
            const vector<vector<string>>& docs = pair.second;

            for (const auto& doc : docs) {
                for (const auto& term : doc) {
                    wordProb[className][term] += intermediate[term][doc[0]]; // 将每个文档首单词作为文档ID
                }
            }
        }

        for (const auto& pair : classDocCount) {
            classProb[pair.first] = static_cast<double>(pair.second) / numDocs;
        }

        for (auto& pair : wordProb) {
            string className = pair.first;
            double totalWords = 0.0;
            for (const auto& wordPair : pair.second) {
                totalWords += wordPair.second;
            }
            for (auto& wordPair : pair.second) {
                wordPair.second = (wordPair.second + 1.0) / (totalWords + wordProb[className].size());
            }
        }
    }

    string classify(const vector<string>& doc) {
        map<string, double> docTFIDF = computeTFIDF(doc, docFreq, numDocs);
        map<string, double> classScores;

        for (const auto& pair : classProb) {
            string className = pair.first;
            classScores[className] = log(pair.second);
            for (const auto& wordPair : docTFIDF) {
                if (wordProb[className].find(wordPair.first) != wordProb[className].end()) {
                    classScores[className] += log(wordProb[className][wordPair.first]);
                }
                else {
                    classScores[className] += log(1.0 / (docTFIDF.size() + wordProb[className].size()));
                }
            }
        }

        return max_element(classScores.begin(), classScores.end(),
            [](const pair<string, double>& a, const pair<string, double>& b) {
                return a.second < b.second;
            })->first;
    }
};

// 随机生成单词
string generateRandomWord(mt19937& gen, uniform_int_distribution<>& dis) {
    int length = dis(gen);
    string word;
    for (int i = 0; i < length; ++i) {
        word += static_cast<char>('a' + dis(gen) % 26);
    }
    return word;
}

// 随机生成句子
string generateRandomSentence(mt19937& gen, uniform_int_distribution<>& dis, int numWords) {
    string sentence;
    for (int i = 0; i < numWords; ++i) {
        sentence += generateRandomWord(gen, dis) + " ";
    }
    return sentence;
}

int main() {
    // 设置随机数生成器
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(3, 10);

    // 自定义训练数据
    map<string, vector<vector<string>>> trainingData = {
        {"spam", {}},
        {"ham", {}}
    };

    // 生成随机的训练数据
    for (int i = 0; i < 10; ++i) {
        trainingData["spam"].push_back(tokenize(generateRandomSentence(gen, dis, 5)));
        trainingData["ham"].push_back(tokenize(generateRandomSentence(gen, dis, 5)));
    }

    NaiveBayesClassifier classifier;
    classifier.train(trainingData);

    // 输入测试数据数量
    int numTestDocs;
    cout << "Enter the number of test documents: ";
    cin >> numTestDocs;
    auto start = std::chrono::high_resolution_clock::now();
    // 生成随机的测试文档
    vector<string> testDocs;
    for (int i = 0; i < numTestDocs; ++i) {
        testDocs.push_back(generateRandomSentence(gen, dis, 5));
    }
      
    for (const auto& doc : testDocs) {
        vector<string> tokens = tokenize(doc);
        string predictedClass = classifier.classify(tokens);
        cout << "Document: " << doc << " => Predicted class: " << predictedClass << endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "总耗时: " << elapsed.count() << " 秒" << std::endl;
    return 0;
}
