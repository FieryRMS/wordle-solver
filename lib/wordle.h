#pragma once

#include <string>
#include <vector>
#include "trie.h"
#include <queue>

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
    Wordle(const string &filepath);
    Wordle(const string &filepath, const string &word);

    // Methods
    bool isWordValid(const string &w);
    Stat guess(const string &guess);
    static string guess2emoji(const vector<TileType> &result);
    bool isGameOver() const { return status != GameStatus::ONGOING; }
    void printPossibleWords() const;
    void printTopNWords(int n);

    // Getters
    int getGuesses() const { return guesses; }
    int getMaxGuesses() const { return maxGuesses; }
    Stat getStat(int i) const;
    string getTargetWord() const { return targetWord; }
    GameStatus getStatus() const { return status; }
    vector<string> getWords(int i) const;
    double getEntropy(int i, string guess) const;
    vector<pair<double, string>> getTopNWords(int n, bool showProgress = false);

    // Setters
    void setTargetWord(const string &word) { targetWord = word; }

   private:
    Trie<N>::Query getUpdatedQuery(const string &guess,
                                   const vector<TileType> &result,
                                   Trie<N>::Query query);


    string targetWord;
    int guesses;
    static const int maxGuesses = 6;
    GameStatus status;
    Trie<N> wordTrie;
    vector<Stat> stats;
    priority_queue<pair<double, string>> wordlist;
};