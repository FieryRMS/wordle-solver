#pragma once

#include <queue>
#include <string>
#include <vector>
#include "trie.h"

using namespace std;

class Wordle {
   public:
    enum class TileType {
        CORRECT,
        WRONG,
        MISPLACED,
        NONE,
    };

    enum class GameStatus {
        ONGOING,
        WON,
        LOST,
    };

   private:
    static const size_t N = 5;
    struct Stat {
        string guess;
        vector<TileType> result;
        int count;
        double patternProb;
        double bits;
        double expectedBits;
        double remainingBits;
        Trie<N>::Query query;
        bool valid = false;

        void print() const;
    };

   public:
    Wordle(const string &allowedFilepath,
           const string &possibleFilepath,
           const string &cacheFilepath = "");

    Wordle(const string &allowedFilepath,
           const string &word,
           const string &possibleFilepath,
           const string &cacheFilepath);

    // Methods
    bool isWordValid(const string &w);
    Stat guess(const string &guess);
    static string guess2emoji(const vector<TileType> &result);
    bool isGameOver() const { return status != GameStatus::ONGOING; }
    void printPossibleWords() const;
    void printTopNWords(int n);
    void reset();
    bool loadCache();
    bool saveCache() const;

    // Getters
    int getGuesses() const { return guesses; }
    int getMaxGuesses() const { return maxGuesses; }
    Stat getStat(int i) const;
    string getTargetWord() const { return targetWord; }
    GameStatus getStatus() const { return status; }
    vector<string> getWords(int i) const;
    double getEntropy(int i, string guess) const;
    vector<pair<double, string>> getTopNWords(const int n,
                                              bool showProgress = false);

    // Setters
    void setTargetWord(const string &word) { targetWord = word; }
    void setRandomTargetWord();

   private:
    Trie<N>::Query getUpdatedQuery(const string &guess,
                                   const vector<TileType> &result,
                                   Trie<N>::Query query);

    struct TopWords {
        int n;
        vector<pair<double, string>> words;
    };
    struct Cache {
        priority_queue<pair<double, string>> wordlistCache;
        map<string, TopWords> TopWordsCache;
        string cachePath;
    };

    string targetWord;
    int guesses;
    static const int maxGuesses = 6;
    GameStatus status;
    Trie<N> wordTrie;
    Trie<N>::ID allowedID = Trie<N>::ID::ALLOWED;
    Trie<N>::ID possibleID = Trie<N>::ID::ALLOWED;
    vector<Stat> stats;
    priority_queue<pair<double, string>> wordlist;
    Cache cache;
};