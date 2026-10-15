#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <cctype>

using namespace std;

// Store informat ion about a document
struct Document
{
    int id;
    string title;
    string content;
};

// Store a document with its relevance score
struct RankedDocument
{
    Document document;
    double score;
};

// Count the frequency of each word
unordered_map<string, int> getWordFrequency(string text)
{
    unordered_map<string, int> frequency;

    for (int i = 0; i < text.length(); i++)
    {
        text[i] = tolower((unsigned char)text[i]);

        if (!isalnum((unsigned char)text[i]))
        {
            text[i] = ' ';
        }
    }

    string word;
    stringstream ss(text);

    while (ss >> word)
    {
        frequency[word]++;
    }

    return frequency;
}

// Calculate Term Frequency
double calculateTF(int wordCount, int totalWords)
{
    if (totalWords == 0)
    {
        return 0.0;
    }

    return (double)wordCount / totalWords;
}

// Calculate Inverse Document Frequency
double calculateIDF(int totalDocuments, int documentsContainingWord)
{
    return log(
        (double)(totalDocuments + 1) /
        (documentsContainingWord + 1)
    ) + 1.0;
}

// Calculate the TF-IDF score of a document
double calculateTFIDF(
    string query,
    const Document& document,
    const vector<Document>& allDocuments)
{
    unordered_map<string, int> queryWords =
        getWordFrequency(query);

    unordered_map<string, int> documentWords =
        getWordFrequency(document.content);

    int totalWords = 0;

    for (auto word : documentWords)
    {
        totalWords += word.second;
    }

    double totalScore = 0.0;
    int totalDocuments = allDocuments.size();

    for (auto word : queryWords)
    {
        string term = word.first;

        int documentFrequency = 0;

        // Count documents containing the query word
        for (const Document& doc : allDocuments)
        {
            unordered_map<string, int> words =
                getWordFrequency(doc.content);

            if (words.find(term) != words.end())
            {
                documentFrequency++;
            }
        }

        // Check whether the current document contains the word
        auto it = documentWords.find(term);

        if (it != documentWords.end())
        {
            double tf = calculateTF(
                it->second,
                totalWords
            );

            double idf = calculateIDF(
                totalDocuments,
                documentFrequency
            );

            totalScore += word.second * tf * idf;
        }
    }

    return totalScore;
}

// Rank documents according to their scores
void rankDocuments(
    string query,
    const vector<Document>& allDocuments)
{
    vector<RankedDocument> results;

    // Calculate the score of every document
    for (const Document& doc : allDocuments)
    {
        double score = calculateTFIDF(
            query,
            doc,
            allDocuments
        );

        // Keep documents that match the query
        if (score > 0.0)
        {
            RankedDocument result;

            result.document = doc;
            result.score = score;

            results.push_back(result);
        }
    }

    // Sort documents from highest score to lowest score
    sort(
        results.begin(),
        results.end(),
        [](const RankedDocument& a, const RankedDocument& b)
        {
            if (a.score == b.score)
            {
                return a.document.id < b.document.id;
            }

            return a.score > b.score;
        }
    );

    // Display the ranked documents
    cout << "\nRanked Search Results\n";
    cout << "---------------------\n";

    if (results.empty())
    {
        cout << "No matching documents found.\n";
        return;
    }

    for (int i = 0; i < results.size(); i++)
    {
        cout << "\nRank: " << i + 1;
        cout << "\nDocument ID: "
             << results[i].document.id;

        cout << "\nTitle: "
             << results[i].document.title;

        cout << "\nRelevance Score: "
             << results[i].score;

        cout << "\n";
    }
}

// Main function for testing Phase 1
int main()
{
    vector<Document> documents =
    {
        {
            1,
            "Machine Learning",
            "Machine learning is used in artificial intelligence"
        },
        {
            2,
            "Learning Algorithms",
            "Machine learning algorithms help computers learn"
        },
        {
            3,
            "Computer Networks",
            "Computer networks connect different computers"
        },
        {
            4,
            "Artificial Intelligence",
            "Artificial intelligence uses machine learning"
        }
    };

    string query;

    cout << "===== Mini Search Engine =====\n";
    cout << "Enter your search query: ";

    getline(cin, query);

    rankDocuments(query, documents);

    return 0;
}
