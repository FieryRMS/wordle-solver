#pragma once
#include "wordle.h"

class Simulator {
   public:
    Simulator(const string &filepath, Wordle &wordle);
    void run(int n);

   private:
    vector<string> words;
    Wordle &wordle;
};