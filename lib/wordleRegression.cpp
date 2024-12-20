#include "wordleRegression.h"
#include <cmath>

using namespace std;

WordleRegression::WordleRegression(const string &allowedFilepath,
                                   const string &possibleFilepath,
                                   const string &cacheFilepath)
    : Wordle(allowedFilepath, possibleFilepath, cacheFilepath)
{}

WordleRegression::WordleRegression(const string &allowedFilepath,
                                   const string &word,
                                   const string &possibleFilepath,
                                   const string &cacheFilepath)
    : Wordle(allowedFilepath, word, possibleFilepath, cacheFilepath)
{}

double WordleRegression::expectedScore(double remainingBits)
{
    // 0.00323876x^{3}-0.0646617x^{2}+0.540225x+0.989117
    // the above equation was derived from the data points from the base wordle game
    return 0.00323876 * pow(remainingBits, 3) -
           0.0646617 * pow(remainingBits, 2) + 0.540225 * remainingBits +
           0.989117;
}

vector<Wordle::Word> WordleRegression::getTopNWords(const int n,
                                                    bool showProgress)
{
    vector<Word> result = Wordle::getTopNWords(n, showProgress);
    auto stat = getStat(-1);
    auto query = stat.query;
    double p = 1.0 / stat.count;
    int guesses = getGuesses() + 1;
    for (auto &word : result)
    {
        if (isInWordSpace(word.word, query))
        {
            // it can possibly be the answer,
            p = 1.0 / stat.count;
        }
        else p = 0;
        word.score =
            p * guesses +
            (1 - p) *
                (guesses + expectedScore(stat.remainingBits - word.entropy));
    }

    auto comp = [&query, this](const Word &a, const Word &b) {
        if (!feq(a.score, b.score)) return a.score < b.score;
        if (isInWordSpace(a.word, query)) return true;
        return false;
    };
    sort(result.begin(), result.end(), comp);
    return result;
}