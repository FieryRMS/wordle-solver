#pragma once
#include "wordle.h"

class Simulator {
   public:
    Simulator(const string &filepath, Wordle &wordle);
    void run();

   private:
    vector<string> words;
    Wordle &wordle;
};