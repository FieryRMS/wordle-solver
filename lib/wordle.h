#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "trie.h"

using namespace std;

class Wordle {
   public:
    enum class TileType {
        CORRECT,
        NONE,
        MISPLACED,
    };

    enum class GameStatus {
        ONGOING,
        WON,
        LOST,
    };

   private:
    static const size_t N = 5;
    struct Stat {
        string guess = "";
        vector<TileType> result = {};
        int count = 0;
        double bits = 0;
        double remainingBits = 0;
        Trie<N>::Query query;
        bool valid = false;
    };

   public:
    Wordle();
    Wordle(const string &word);

    // Methods
    bool isWordValid(const string &w);
    Stat guess(const string &guess);
    static string guess2emoji(const vector<TileType> &result);
    bool isGameOver() const { return status != GameStatus::ONGOING; }

    // Getters
    int getGuesses() const { return guesses; }
    int getMaxGuesses() const { return maxGuesses; }
    Stat getStat(int i) const;
    string getTargetWord() const { return targetWord; }
    GameStatus getStatus() const { return status; }
    vector<string> getWords(int i) const;

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
    Trie<N> wordlist;
    vector<Stat> stats;
};