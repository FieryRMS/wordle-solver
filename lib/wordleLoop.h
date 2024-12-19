#pragma once
#include <list>
#include <string>
#include "Wordle.h"

using namespace std;

class WordleLoop : public Wordle {
   public:
    WordleLoop(const string &allowedFilepath,
               const string &possibleFilepath,
               const string &cacheFilepath = "");
    WordleLoop(const string &allowedFilepath,
               const string &word,
               const string &possibleFilepath,
               const string &cacheFilepath);

    unordered_map<string, int> getPatternsCounts(const string &guess,
                                       Trie<N>::Query query) const override;
    int getQueryCount(Trie<N>::Query query) const override;
    void reset() override;
    bool loadPatternCache();
    bool savePatternCache() const;

   protected:
    Trie<N>::Query getUpdatedQuery(const string &guess,
                                   const string &pattern,
                                   Trie<N>::Query query) override;

   private:
    struct Cache {
        list<string> words;
        unordered_map<string, string> patterns;
    };
    list<string> words;

    Cache cache;
};