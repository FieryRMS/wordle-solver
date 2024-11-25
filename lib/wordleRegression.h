#pragma once
#include "wordle.h"

using namespace std;

class WordleRegression : public Wordle {
   public:
    WordleRegression(const string &allowedFilepath,
                     const string &possibleFilepath,
                     const string &cacheFilepath = "");

    WordleRegression(const string &allowedFilepath,
                     const string &word,
                     const string &possibleFilepath,
                     const string &cacheFilepath);
    vector<Word> getTopNWords(const int n, bool showProgress = false) override;

   private:
    double expectedScore(double remainingBits);
};