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

    Wordle();
    Wordle(const string &word);

    // Methods
    bool isWordValid(const string &w);
    vector<TileType> guess(const string &guess);
    static string guess2emoji(const vector<TileType> &result);
    bool isGameOver() const { return status != GameStatus::ONGOING; }

    // Getters
    int getGuesses() const { return guesses; }
    int getMaxGuesses() const { return maxGuesses; }
    string getTargetWord() const { return targetWord; }
    GameStatus getStatus() const { return status; }

    // Setters
    void setTargetWord(const string &word) { targetWord = word; }

   private:
    static const size_t N = 5;
    string targetWord;
    int guesses;
    int maxGuesses = 6;
    GameStatus status;
    Trie<N> wordlist;
};