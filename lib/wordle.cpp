#include "wordle.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include "ProgressBar.h"

using namespace std;

const int titleWidth = 23, numWidth = 5;
const string EntropyCache = "entropy_cache.txt";

bool feq(double a, double b)
{
    return fabs(a - b) < 1e-6;
}

Wordle::Wordle(const string &filepath,
               const string &possibleFilepath,
               const string &cacheFilepath)
    : Wordle(filepath,
             "",
             possibleFilepath,
             cacheFilepath.empty() ? EntropyCache : cacheFilepath)
{
    // choose a random word from the list
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, wordTrie.count("", possibleID));
    targetWord = wordTrie.getNthWord(dis(gen), possibleID);
}

Wordle::Wordle(const string &filepath,
               const string &targetWord,
               const string &possibleFilepath,
               const string &cacheFilepath)
    : targetWord(targetWord),
      guesses(0),
      status(GameStatus::ONGOING),
      wordTrie()
{
    stats.reserve(maxGuesses + 1);

    // check if cache exists
    ifstream cache(cacheFilepath);
    if (cache.is_open())
    {
        string word;
        double entropy;
        while (cache >> word >> entropy)
        {
            wordTrie.insert(word, allowedID);
            wordlist.push({ entropy, word });
        }
        cache.close();

        cout << "WARN: Using cached entropy values from "
                "file: "
             << filesystem::absolute(cacheFilepath) << endl;
    }
    else
    {
        ifstream file(filepath);
        if (!file.is_open())
        {
            cerr << "Error opening file: " << filepath << endl;
            exit(1);
        }

        string word;
        while (file >> word)
        {
            wordTrie.insert(word, allowedID);
            wordlist.push({ -1, word });
        }
        file.close();
    }

    if (!possibleFilepath.empty())
    {
        possibleID = Trie<N>::ID::POSSIBLE;
        ifstream file(possibleFilepath);
        if (!file.is_open())
        {
            cerr << "Error opening file: " << possibleFilepath << endl;
            exit(1);
        }

        string word;
        while (file >> word) wordTrie.insert(word, possibleID);
    }

    int count = wordTrie.count("", possibleID);

    stats.push_back({
        .guess = "",
        .result = {},
        .count = count,
        .patternProb = 0,
        .bits = 0,
        .expectedBits = 0,
        .remainingBits = log2(count),
        .query = wordTrie.query("", possibleID),
        .valid = true,
    });

    cout << "Pre-calculating entropy..." << endl;
    getTopNWords(0, true);
    // save cache
    ofstream cache_out(cacheFilepath);
    auto wordlist_copy = wordlist;
    while (!wordlist_copy.empty())
    {
        auto word = wordlist_copy.top();
        wordlist_copy.pop();
        cache_out << word.second << " " << setprecision(17) << word.first
                  << endl;
    }
}

bool Wordle::isWordValid(const string &word)
{
    if (word.size() != N) return false;

    for (auto &c : word)
        if (!islower(c)) return false;

    // if not in wordlist return false
    if (wordTrie.count(word, allowedID) == 1) return true;
    return false;
}

Wordle::Stat Wordle::guess(const string &guess)
{
    // return invalid stat
    if (isGameOver())
        return Stat({
            .guess = guess,
            .result = {},
            .count = 0,
            .patternProb = 0,
            .bits = 0,
            .expectedBits = 0,
            .remainingBits = 0,
            .query = wordTrie.query("", allowedID),
            .valid = false,
        });

    vector<TileType> result(N, TileType::WRONG);
    vector<bool> visited(N, false);

    // check for correct letters
    int cnt = 0;
    for (int i = 0; i < N; i++)
    {
        if (targetWord[i] == guess[i])
        {
            result[i] = TileType::CORRECT;
            visited[i] = true;
            cnt++;
        }
    }

    // check for misplaced letters
    for (int i = 0; i < N; i++)
    {
        if (result[i] == TileType::CORRECT) continue;
        for (int j = 0; j < targetWord.size(); j++)
        {
            if (visited[j]) continue;
            if (targetWord[j] == guess[i])
            {
                result[i] = TileType::MISPLACED;
                visited[j] = true;
                break;
            }
        }
    }

    guesses++;
    auto query = getUpdatedQuery(guess, result, getStat(-1).query);
    int count = wordTrie.count(query), prevCount = stats.back().count;

    // Information = log2(1 / P(x)) = - log2(P(x)) = - log2(count / prevCount) = log2(prevCount) - log2(count)
    double bits = log2(prevCount) - log2(count);

    stats.push_back({
        .guess = guess,
        .result = result,
        .count = count,
        .patternProb = (double)count / prevCount,
        .bits = bits,
        .expectedBits = getEntropy(-1, guess),
        .remainingBits = log2(count),
        .query = query,
        .valid = true,
    });

    // query.print(); // for debugging
    if (cnt == targetWord.size()) status = GameStatus::WON;
    else if (guesses == maxGuesses) status = GameStatus::LOST;

    return stats.back();
}

string Wordle::guess2emoji(const vector<TileType> &result)
{
    string emojis;
    for (auto &tile : result)
    {
        switch (tile)
        {
            case TileType::CORRECT:
                emojis += "🟩";
                break;
            case TileType::MISPLACED:
                emojis += "🟨";
                break;
            case TileType::WRONG:
                emojis += "🟥";
                break;
            default:
                emojis += "⬜";
        }
    }
    return emojis;
}

Trie<Wordle::N>::Query Wordle::getUpdatedQuery(const string &guess,
                                               const vector<TileType> &result,
                                               Trie<N>::Query query)
{
    string includes = "";
    for (int i = 0; i < N; i++)
    {
        switch (result[i])
        {
            case TileType::CORRECT:
                query.setCorrect(guess[i], i);
                break;
            case TileType::MISPLACED:
                query.setMisplaced(guess[i], i);
                includes += guess[i];
                break;
            case TileType::WRONG:
                query.exclude(guess[i]);
                break;
            default:
                throw invalid_argument("Invalid tile type");
        }
    }
    query.include(includes);
    return query;
}

Wordle::Stat Wordle::getStat(int i) const
{
    if (i == -1) return stats.back();
    return stats.at(i);
}

vector<string> Wordle::getWords(int i) const
{
    vector<string> result;
    wordTrie.count(getStat(i).query, &result);
    return result;
}

double Wordle::getEntropy(int i, string guess) const
{
    auto stat = getStat(i);
    auto patterns = wordTrie.getPatternsCounts(guess, stat.query);
    int total = stat.count;
    // E = sum P(x) * log2(1 / P(x)) where x is the pattern
    // log2(1 / P(x)) = - log2(P(x)) = - log2(count / total) = log2(total) - log2(count)
    double expectedBits = 0;
    for (auto &pattern : patterns)
    {
        double prob = (double)pattern.second / total;
        expectedBits += prob * (log2(total) - log2(pattern.second));
    }
    return expectedBits;
}
void Wordle::Stat::print() const
{
    if (!valid)
    {
        cout << "Invalid stat" << endl;
        return;
    }

    cout << setw(titleWidth) << "GUESS: " << guess << endl;
    cout << setw(titleWidth) << "PATTERN: " << guess2emoji(result) << endl;
    cout << setw(titleWidth) << "REMAINING WORDS: " << setw(numWidth) << count
         << endl;
    cout << setw(titleWidth) << "PATTERN PROBABILITY: " << setw(numWidth)
         << fixed << setprecision(2) << patternProb << endl;
    cout << setw(titleWidth) << "INFORMATION GAINED: " << setw(numWidth)
         << fixed << setprecision(2) << bits << " bits" << endl;
    cout << setw(titleWidth) << "EXPECTED GAIN: " << setw(numWidth) << fixed
         << setprecision(2) << expectedBits << " bits" << endl;
    cout << setw(titleWidth) << "REMAINING INFORMATION: " << setw(numWidth)
         << fixed << setprecision(2) << remainingBits << " bits" << endl;
}

vector<pair<double, string>> Wordle::getTopNWords(int n, bool showProgress)
{
    // we assume it is sorted
    // we first calculate for uninitialized words
    // when recalculating
    // we know that entropy can never be more than the previous entropy,
    // therefore best case senario new=prev
    // So we can skip when we have n elements, and the smallest element is >= next elements old entropy
    ProgressBar progressBar(wordlist.size(), 70);
    if (showProgress) progressBar.update(0);

    priority_queue<double> topEntropy;
    vector<pair<double, string>> updatedWords;
    auto query = getStat(-1).query;

    auto isInWordSpace = [&query, this](const string &word) {
        // if it exists in the possible words and it matches the query
        return wordTrie.count(word, possibleID) && query.verify(word);
    };

    for (int i = 0; !wordlist.empty(); i++)
    {
        // either -1 (uninitialized) or not enough words
        // or the smallest updated entropy in top n (topEntropy)
        // is less than the next word's old entropy (wordlist)
        // [meaning we can possibly get a better entropy]
        if (feq(wordlist.top().first, -1) ||
            (n != 0 && (topEntropy.size() < n ||
                        wordlist.top().first > -topEntropy.top())))
        {
            auto word = wordlist.top();
            wordlist.pop();
            word.first = getEntropy(-1, word.second);  // expensive
            if (feq(word.first, 0) && !isInWordSpace(word.second)) continue;
            topEntropy.push(-word.first);
            if (topEntropy.size() > n) topEntropy.pop();
            updatedWords.push_back(word);
        }
        else break;

        if (showProgress) progressBar.update(i + 1);
    }

    for (auto &word : updatedWords) wordlist.push(word);

    auto comp = [&isInWordSpace](const pair<double, string> &a,
                                 const pair<double, string> &b) {
        if (!feq(a.first, b.first)) return a.first < b.first;
        if (isInWordSpace(a.second)) return false;
        return true;
    };
    priority_queue<pair<double, string>, vector<pair<double, string>>,
                   decltype(comp)>
        topWords(comp);

    // if n=0 skip, else get top n words and include scores that are equal to
    // the last element (it may contain a word in the query)
    while (
        n != 0 && !wordlist.empty() &&
        (topWords.size() < n || feq(wordlist.top().first, -topEntropy.top())))
        topWords.push(wordlist.top()), wordlist.pop();

    vector<pair<double, string>> result;
    result.reserve(n);
    while (!topWords.empty())
    {
        auto word = topWords.top();
        topWords.pop();
        if (feq(word.first, 0) && !isInWordSpace(word.second)) continue;
        if (n) result.push_back(word), n--;
        wordlist.push(word);
    }

    if (showProgress) progressBar.finish();
    return result;
}

void Wordle::printPossibleWords() const
{
    vector<string> result = getWords(-1);
    cout << setw(titleWidth) << "POSSIBILITIES: " << "{ ";
    for (auto &word : result) cout << '"' << word << "\", ";
    cout << "}" << endl;
}

void Wordle::printTopNWords(int n)
{
    cout << "Calculating top " << n << " words..." << endl;
    auto topWords = getTopNWords(n, true);
    string title = "TOP " + to_string(n) + " WORDS: ";
    cout << setw(titleWidth) << title << "WORDS | ENTROPY" << endl;
    for (auto &word : topWords)
        cout << setw(titleWidth) << "" << word.second << " | " << setw(numWidth)
             << fixed << setprecision(2) << word.first << endl;
}
