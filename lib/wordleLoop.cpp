#include "wordleLoop.h"
#include <fstream>
#include <iostream>
#include "ProgressBar.h"

using namespace std;

WordleLoop::WordleLoop(const string &allowedFilepath,
                       const string &possibleFilepath,
                       const string &cacheFilepath)
    : WordleLoop(allowedFilepath, "", possibleFilepath, cacheFilepath)
{
    setRandomTargetWord();
}

WordleLoop::WordleLoop(const string &allowedFilepath,
                       const string &word,
                       const string &possibleFilepath,
                       const string &cacheFilepath)
    : Wordle(allowedFilepath, word, possibleFilepath, cacheFilepath)
{
    ifstream possibleFile(possibleFilepath);

    if (!possibleFile.is_open())
    {
        cerr << "Error opening file: " << allowedFilepath << endl;
        exit(1);
    }

    string possibleWord;
    while (possibleFile >> possibleWord) words.push_back(possibleWord);
    possibleFile.close();

    copy(words.begin(), words.end(), back_inserter(cache.words));

    // calculate all patterns
    if (!loadPatternCache())
    {
        ifstream allowedFile(allowedFilepath);
        if (!allowedFile.is_open())
        {
            cerr << "Error opening file: " << allowedFilepath << endl;
            exit(1);
        }
        string allowedWord;
        list<string> guesses;
        while (allowedFile >> allowedWord) guesses.push_back(allowedWord);
        allowedFile.close();

        cout << "Calculating patterns..." << endl;
        ProgressBar progressBar(1ull * words.size() * guesses.size());
        int i = 0;
        progressBar.update(i);
        for (auto &target : words)
            for (auto &guess : guesses)
            {
                cache.patterns[guess + target] = getPattern(guess, target);
                progressBar.update(++i);
            }
        progressBar.finish();
    }

    // probably dont save it since its large, numpy i think compresses the output, and also 3b1b used a number to represent the pattern
    // savePatternCache();
}

bool WordleLoop::savePatternCache() const
{
    ofstream cacheFile("patterns.txt");
    if (!cacheFile.is_open()) return false;

    for (auto &pattern : cache.patterns)
        cacheFile << pattern.first << " " << pattern.second << endl;

    cacheFile.close();
    return true;
}

bool WordleLoop::loadPatternCache()
{
    ifstream cacheFile("patterns.txt");
    if (!cacheFile.is_open()) return false;
    cout << "Using cached patterns..." << endl;
    string word, pattern;
    while (cacheFile >> word >> pattern) cache.patterns[word] = pattern;

    cacheFile.close();
    return true;
}

/**
 * hacky solution, this functions shouldve been a const function
 * @see WordleLoop::getQueryCount
 * @see WordleLoop::getPatternsCounts
 */
Trie<Wordle::N>::Query WordleLoop::getUpdatedQuery(const string &guess,
                                                   const string &pattern,
                                                   Trie<N>::Query query)
{
    auto newQuery = Wordle::getUpdatedQuery(guess, pattern, query);

    words.remove_if([&guess, &pattern, this](const string &word) {
        return cache.patterns.at(guess + word) != pattern;
    });

    return newQuery;
}

void WordleLoop::reset()
{
    Wordle::reset();
    words.clear();
    copy(cache.words.begin(), cache.words.end(), back_inserter(words));
}

/**
 * @brief may cause runtime bugs as it does not depend on the query, only works because super class only ever accesses the most recent query
 */
int WordleLoop::getQueryCount(Trie<N>::Query query) const
{
    return words.size();
}

/**
 * @brief may cause runtime bugs as it does not depend on the query, only works because super class only ever accesses the most recent query
 */
map<string, int> WordleLoop::getPatternsCounts(const string &guess,
                                               Trie<N>::Query query) const
{
    map<string, int> patterns;
    for (auto &word : words)
    {
        string pattern = cache.patterns.at(guess + word);
        patterns[pattern]++;
    }

    return patterns;
}