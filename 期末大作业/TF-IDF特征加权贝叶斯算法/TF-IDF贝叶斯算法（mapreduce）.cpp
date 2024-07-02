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

// �ִʺ��������ı����Ϊ����
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    stringstream ss(text);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// �����Ƶ
map<string, int> computeWordFrequencies(const vector<string>& tokens) {
    map<string, int> wordFreq;
    for (const string& word : tokens) {
        wordFreq[word]++;
    }
    return wordFreq;
}

// ����TF-IDFȨ��
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
    map<string, map<string, double>> wordProb; // ÿ�������ÿ�����ʵ���������
    map<string, double> classProb; // ÿ�������������
    map<string, int> docFreq; // ÿ�������ڶ��ٸ��ĵ��г��ֹ�
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

        // ʹ��MapReduce����TF-IDF
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
                        intermediate[pair.first][doc[0]] += pair.second; // ��ÿ���ĵ��׵�����Ϊ�ĵ�ID
                    }
                }
            }
            };

        for (int i = 0; i < 4; ++i) { // ����ʹ��4���߳�
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
                    wordProb[className][term] += intermediate[term][doc[0]]; // ��ÿ���ĵ��׵�����Ϊ�ĵ�ID
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

// ������ɵ���
string generateRandomWord(mt19937& gen, uniform_int_distribution<>& dis) {
    int length = dis(gen);
    string word;
    for (int i = 0; i < length; ++i) {
        word += static_cast<char>('a' + dis(gen) % 26);
    }
    return word;
}

// ������ɾ���
string generateRandomSentence(mt19937& gen, uniform_int_distribution<>& dis, int numWords) {
    string sentence;
    for (int i = 0; i < numWords; ++i) {
        sentence += generateRandomWord(gen, dis) + " ";
    }
    return sentence;
}

int main() {
    // ���������������
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(3, 10);

    // �Զ���ѵ������
    map<string, vector<vector<string>>> trainingData = {
        {"spam", {}},
        {"ham", {}}
    };

    // ���������ѵ������
    for (int i = 0; i < 10; ++i) {
        trainingData["spam"].push_back(tokenize(generateRandomSentence(gen, dis, 5)));
        trainingData["ham"].push_back(tokenize(generateRandomSentence(gen, dis, 5)));
    }

    NaiveBayesClassifier classifier;
    classifier.train(trainingData);

    // ���������������
    int numTestDocs;
    cout << "Enter the number of test documents: ";
    cin >> numTestDocs;
    auto start = std::chrono::high_resolution_clock::now();
    // ��������Ĳ����ĵ�
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
    std::cout << "�ܺ�ʱ: " << elapsed.count() << " ��" << std::endl;
    return 0;
}
