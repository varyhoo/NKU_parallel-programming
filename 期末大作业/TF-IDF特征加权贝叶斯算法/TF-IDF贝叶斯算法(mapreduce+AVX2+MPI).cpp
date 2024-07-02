#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <cmath>
#include <mpi.h>
#include <immintrin.h>
#include <random>
#include <algorithm>

using namespace std;

const int MASTER = 0;

// 分词函数
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    istringstream stream(text);
    string word;
    while (stream >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

// 计算词频
unordered_map<string, int> computeWordFrequencies(const vector<string>& tokens) {
    unordered_map<string, int> wordFreq;
    for (const string& word : tokens) {
        wordFreq[word]++;
    }
    return wordFreq;
}

// 计算TF-IDF
unordered_map<string, double> computeTFIDF(const vector<string>& tokens, const unordered_map<string, int>& docFreq, int numDocs) {
    unordered_map<string, int> termFreq = computeWordFrequencies(tokens);
    unordered_map<string, double> tfidf;
    for (const auto& term : termFreq) {
        double tf = 1.0 * term.second / tokens.size();
        double idf = log(1.0 * numDocs / (1 + docFreq.at(term.first)));
        tfidf[term.first] = tf * idf;
    }
    return tfidf;
}

// 随机生成单词
string generateRandomWord(mt19937& gen, uniform_int_distribution<int>& dist) {
    int len = dist(gen);
    string word;
    for (int i = 0; i < len; ++i) {
        word += 'a' + dist(gen) % 26;
    }
    return word;
}

// 随机生成句子
string generateRandomSentence(mt19937& gen, uniform_int_distribution<int>& dist, int numWords) {
    string sentence;
    for (int i = 0; i < numWords; ++i) {
        if (i > 0) sentence += " ";
        sentence += generateRandomWord(gen, dist);
    }
    return sentence;
}

// 朴素贝叶斯分类器
class NaiveBayesClassifier {
public:
    void train(const vector<pair<string, string>>& trainingData) {
        int numDocs = trainingData.size();
        unordered_map<string, int> docFreq;
        for (const auto& doc : trainingData) {
            vector<string> tokens = tokenize(doc.second);
            unordered_map<string, int> wordFreq = computeWordFrequencies(tokens);
            for (const auto& word : wordFreq) {
                docFreq[word.first]++;
            }
        }

        MPI_Bcast(&numDocs, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        int numWords = docFreq.size();
        MPI_Bcast(&numWords, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        vector<string> allWords;
        vector<int> allFreqs;
        if (rank == MASTER) {
            for (const auto& entry : docFreq) {
                allWords.push_back(entry.first);
                allFreqs.push_back(entry.second);
            }
        }

        // Broadcast word frequencies to all processes
        MPI_Bcast(allWords.data(), numWords, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        MPI_Bcast(allFreqs.data(), numWords, MPI_INT, MASTER, MPI_COMM_WORLD);

        for (int i = 0; i < numWords; ++i) {
            docFreq[allWords[i]] = allFreqs[i];
        }

        for (const auto& doc : trainingData) {
            vector<string> tokens = tokenize(doc.second);
            unordered_map<string, double> tfidf = computeTFIDF(tokens, docFreq, numDocs);
            for (const auto& term : tfidf) {
                wordProb[doc.first][term.first] += term.second;
            }
            classProb[doc.first]++;
        }

        for (auto& entry : classProb) {
            entry.second = log(entry.second / numDocs);
        }

        for (auto& classEntry : wordProb) {
            for (auto& wordEntry : classEntry.second) {
                wordEntry.second = log((wordEntry.second + 1) / (classProb[classEntry.first] + numWords));
            }
        }
    }

    string classify(const string& doc) {
        vector<string> tokens = tokenize(doc);
        unordered_map<string, double> tfidf = computeTFIDF(tokens, docFreq, numDocs);
        unordered_map<string, double> classScores;

        for (const auto& entry : classProb) {
            classScores[entry.first] = entry.second;
            for (const auto& term : tfidf) {
                if (wordProb[entry.first].find(term.first) != wordProb[entry.first].end()) {
                    classScores[entry.first] += wordProb[entry.first][term.first] * term.second;
                }
                else {
                    classScores[entry.first] += log(1.0 / (classProb[entry.first] + numWords)) * term.second;
                }
            }
        }

        return max_element(classScores.begin(), classScores.end(),
            [](const pair<string, double>& a, const pair<string, double>& b) { return a.second < b.second; })->first;
    }

private:
    unordered_map<string, unordered_map<string, double>> wordProb;
    unordered_map<string, double> classProb;
    unordered_map<string, int> docFreq;
    int numDocs;
    int rank, size;
};

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(3, 10);

    vector<pair<string, string>> trainingData;
    if (rank == MASTER) {
        trainingData = {
            {"class1", generateRandomSentence(gen, dist, 100)},
            {"class2", generateRandomSentence(gen, dist, 100)},
            {"class3", generateRandomSentence(gen, dist, 100)}
        };
    }

    NaiveBayesClassifier classifier;
    classifier.train(trainingData);

    if (rank == MASTER) {
        int numTestDocs;
        cout << "Enter number of test documents: ";
        cin >> numTestDocs;
        vector<string> testDocs;
        for (int i = 0; i < numTestDocs; ++i) {
            testDocs.push_back(generateRandomSentence(gen, dist, 100));
        }

        for (const string& doc : testDocs) {
            cout << "Document: " << doc << "\nClass: " << classifier.classify(doc) << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
