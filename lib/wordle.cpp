#include "wordle.h"
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

Wordle::Wordle(const string &filepath,
               const string &possibleFilepath,
               const string &cacheFilepath)
    : Wordle(filepath,
             "",
             possibleFilepath,
             cacheFilepath.empty() ? EntropyCache : cacheFilepath)
{
    setRandomTargetWord();
}

Wordle::Wordle(const string &allowedFilepath,
               const string &targetWord,
               const string &possibleFilepath,
               const string &cacheFilepath)
    : targetWord(targetWord),
      guesses(0),
      status(GameStatus::ONGOING),
      wordTrie()
{
    stats.reserve(maxGuesses + 1);
    cache.cachePath = cacheFilepath;

    // check if cache exists
    bool cached = loadCache();
    if (cached) wordlist = cache.wordlistCache;

    ifstream allowedFile(allowedFilepath);
    if (!allowedFile.is_open())
    {
        cerr << "Error opening file: " << allowedFilepath << endl;
        exit(1);
    }

    string allowedWord;
    while (allowedFile >> allowedWord)
    {
        wordTrie.insert(allowedWord, allowedID);
        if (!cached)
            wordlist.push({
                .word = allowedWord,
                .score = -1,
                .entropy = -1,
                .maxEntropy = -1,
            });
    }
    allowedFile.close();

    if (!possibleFilepath.empty())
    {
        possibleID = Trie<N>::ID::POSSIBLE;
        ifstream possibleFile(possibleFilepath);
        if (!possibleFile.is_open())
        {
            cerr << "Error opening file: " << possibleFilepath << endl;
            exit(1);
        }

        string possibleWord;
        while (possibleFile >> possibleWord)
            wordTrie.insert(possibleWord, possibleID);
    }

    int count = wordTrie.count("", possibleID);

    stats.push_back({
        .guess = "",
        .pattern = "",
        .count = count,
        .patternProb = 0,
        .bits = 0,
        .entropy = 0,
        .remainingBits = log2(count),
        .query = wordTrie.query("", possibleID),
        .valid = true,
    });

    // save cache
    if (!cached)
    {
        cout << "Pre-calculating entropy..." << endl;
        getTopNWords(0, true);
        cache.wordlistCache = wordlist;
        saveCache();
    }
}

bool Wordle::loadCache()
{
    fstream cacheFile(cache.cachePath, ios::in);
    if (!cacheFile.is_open()) return false;

    cout << "WARN: Using cached entropy values from "
            "file: "
         << filesystem::absolute(cache.cachePath) << endl;

    string word;
    double score, entropy, maxEntropy;
    while (cacheFile >> word >> score >> entropy >> maxEntropy &&
           word != "#####")
        cache.wordlistCache.push({
            .word = word,
            .score = score,
            .entropy = entropy,
            .maxEntropy = maxEntropy,
        });

    string query;
    int n, count;
    while (cacheFile >> query >> n >> count)
    {
        vector<Word> words;
        words.reserve(count);
        for (int i = 0; i < count; i++)
        {
            cacheFile >> word >> score >> entropy >> maxEntropy;
            words.push_back({
                .word = word,
                .score = score,
                .entropy = entropy,
                .maxEntropy = maxEntropy,
            });
        }
        cache.TopWordsCache[query] = {
            .n = n,
            .words = words,
        };
    }
    cacheFile.close();

    return true;
}

bool Wordle::saveCache() const
{
    fstream cacheFile(cache.cachePath, ios::out | ios::trunc);
    if (!cacheFile.is_open()) return false;

    auto cp = cache.wordlistCache;
    while (!cp.empty())
    {
        auto word = cp.top();
        cp.pop();
        cacheFile << word.word << " " << setprecision(17) << word.score << " "
                  << setprecision(17) << word.entropy << " " << setprecision(17)
                  << word.maxEntropy << endl;
    }

    cacheFile << "##### -1 -1 -1" << endl;

    for (auto &[query, topWords] : cache.TopWordsCache)
    {
        cacheFile << query << " " << topWords.n << " " << topWords.words.size()
                  << endl;
        for (auto &word : topWords.words)
            cacheFile << word.word << " " << setprecision(17) << word.score
                      << " " << setprecision(17) << word.entropy << " "
                      << setprecision(17) << word.maxEntropy << " ";
        cacheFile << endl;
    }

    cacheFile.close();

    return true;
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
            .pattern = "",
            .count = 0,
            .patternProb = 0,
            .bits = 0,
            .entropy = 0,
            .remainingBits = 0,
            .query = wordTrie.query("", allowedID),
            .valid = false,
        });

    string pattern(N, TileType::WRONG);
    vector<bool> visited(N, false);

    // check for correct letters
    int cnt = 0;
    for (int i = 0; i < N; i++)
    {
        if (targetWord[i] == guess[i])
        {
            pattern[i] = TileType::CORRECT;
            visited[i] = true;
            cnt++;
        }
    }

    // check for misplaced letters
    for (int i = 0; i < N; i++)
    {
        if (pattern[i] == TileType::CORRECT) continue;
        for (int j = 0; j < targetWord.size(); j++)
        {
            if (visited[j]) continue;
            if (targetWord[j] == guess[i])
            {
                pattern[i] = TileType::MISPLACED;
                visited[j] = true;
                break;
            }
        }
    }

    guesses++;
    auto query = getUpdatedQuery(guess, pattern, getStat(-1).query);
    int count = wordTrie.count(query), prevCount = stats.back().count;

    // Information = log2(1 / P(x)) = - log2(P(x)) = - log2(count / prevCount) = log2(prevCount) - log2(count)
    double bits = log2(prevCount) - log2(count);

    stats.push_back({
        .guess = guess,
        .pattern = pattern,
        .count = count,
        .patternProb = (double)count / prevCount,
        .bits = bits,
        .entropy = getEntropy(-1, guess).entropy,
        .remainingBits = log2(count),
        .query = query,
        .valid = true,
    });

    // query.print(); // for debugging
    if (cnt == targetWord.size()) status = GameStatus::WON;
    else if (guesses == maxGuesses) status = GameStatus::LOST;

    return stats.back();
}

string Wordle::guess2emoji(const string &pattern)
{
    string emojis;
    for (auto &tile : pattern)
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
                                               const string &pattern,
                                               Trie<N>::Query query)
{
    string includes = "";
    for (int i = 0; i < N; i++)
    {
        switch (pattern[i])
        {
            case TileType::CORRECT:
                query.setCorrect(guess[i], i);
                includes += guess[i];
                break;
            case TileType::MISPLACED:
                query.setMisplaced(guess[i], i);
                includes += guess[i];
                break;
            case TileType::WRONG:
                query.exclude(guess[i]);
                query.setMisplaced(guess[i], i);
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

Wordle::Word Wordle::getEntropy(int i, string guess) const
{
    auto stat = getStat(i);
    auto patterns = wordTrie.getPatternsCounts(guess, stat.query);
    int total = stat.count;
    // E = sum P(x) * log2(1 / P(x)) where x is the pattern
    // log2(1 / P(x)) = - log2(P(x)) = - log2(count / total) = log2(total) - log2(count)
    double entropy = 0, maxEntropy = log2(patterns.size());
    for (auto &pattern : patterns)
    {
        double prob = (double)pattern.second / total;
        entropy += prob * (log2(total) - log2(pattern.second));
    }

    return {
        .word = guess,
        .score = entropy,
        .entropy = entropy,
        .maxEntropy = maxEntropy,
    };
}
void Wordle::Stat::print() const
{
    if (!valid)
    {
        cout << "Invalid stat" << endl;
        return;
    }

    cout << setw(titleWidth) << "GUESS: " << guess << endl;
    cout << setw(titleWidth) << "PATTERN: " << guess2emoji(pattern) << endl;
    cout << setw(titleWidth) << "REMAINING WORDS: " << setw(numWidth) << count
         << endl;
    cout << setw(titleWidth) << "PATTERN PROBABILITY: " << setw(numWidth)
         << fixed << setprecision(2) << patternProb << endl;
    cout << setw(titleWidth) << "INFORMATION GAINED: " << setw(numWidth)
         << fixed << setprecision(2) << bits << " bits" << endl;
    cout << setw(titleWidth) << "ENTROPY: " << setw(numWidth) << fixed
         << setprecision(2) << entropy << " bits" << endl;
    cout << setw(titleWidth) << "REMAINING INFORMATION: " << setw(numWidth)
         << fixed << setprecision(2) << remainingBits << " bits" << endl;
}

vector<Wordle::Word> Wordle::getTopNWords(const int n, bool showProgress)
{
    // we assume it is sorted
    // we first calculate for uninitialized words
    // when recalculating
    // we know that entropy can never be more than the previous entropy,
    // therefore best case senario new=prev
    // So we can skip when we have n elements, and the smallest element is >= next elements old entropy
    // TRY 2:
    // the above idea was slightly wrong. Entropy can increase if even when we remove information
    // but each word can get a max entropy when all patterns have equal probability, ie the words are equally distributed
    // among all the patterns.
    // since number of words can only decrease, max entropy can only decrease also.
    // max entropy is just log2(number of patterns)
    ProgressBar progressBar(wordlist.size());
    if (showProgress) progressBar.update(0);

    auto query = getStat(-1).query;

    // check if result exists in cache
    if (cache.TopWordsCache.contains(query.serialize()))
    {
        auto &result = cache.TopWordsCache[query.serialize()];
        if (result.n >= n || result.words.size() < result.n)
        {
            if (showProgress)
            {
                progressBar.finish();
                cout << "cache hit!" << endl;
            }
            return result.words;
        }
    }

    auto comp = [&query, this](const Word &a, const Word &b) {
        if (!feq(a.score, b.score)) return a.score > b.score;
        if (isInWordSpace(a.word, query)) return true;
        return false;
    };

    priority_queue<Word, vector<Word>, decltype(comp)> topWords(comp);
    vector<Word> updatedWords;
    for (int i = 0; !wordlist.empty(); i++)
    {
        // either -1 (uninitialized) or not enough words
        // or the smallest updated entropy in top n (topEntropy)
        // is leq the next word's max entropy (wordlist)
        // [meaning we can possibly get a better or equal entropy]
        // [equal because we need to rank words in search space higher]
        if (feq(wordlist.top().maxEntropy, -1) ||
            (n != 0 && (topWords.size() < n ||
                        wordlist.top().maxEntropy >= topWords.top().entropy)))
        {
            auto word = wordlist.top();
            wordlist.pop();
            word = getEntropy(-1, word.word);  // expensive
            if (feq(word.maxEntropy, 0) && !isInWordSpace(word.word, query))
                continue;
            topWords.push(word);
            if (topWords.size() > n) topWords.pop();
            updatedWords.push_back(word);
        }
        else break;

        if (showProgress) progressBar.update(i + 1);
    }

    for (auto &word : updatedWords) wordlist.push(word);

    vector<Word> result;
    result.reserve(n);
    while (!topWords.empty())
    {
        result.push_back(topWords.top());
        topWords.pop();
    }
    reverse(result.begin(), result.end());

    if (n != 0)
        cache.TopWordsCache[query.serialize()] = {
            .n = n,
            .words = result,
        };

    if (showProgress) progressBar.finish();
    return result;
}

bool Wordle::isInWordSpace(const string &word, Trie<N>::Query &query) const
{
    // if it exists in the possible words and it matches the query
    return wordTrie.count(word, possibleID) && query.verify(word);
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
    cout << setw(titleWidth) << title << "WORDS | SCORE | ENTROPY" << endl;
    for (auto &word : topWords)
    {
        cout << setw(titleWidth) << "" << word.word << " | " << setw(numWidth)
             << fixed << setprecision(2) << word.score << " | "
             << setw(numWidth) << fixed << setprecision(2) << word.entropy
             << endl;
        n--;
        if (n == 0) break;
    }
}

void Wordle::setRandomTargetWord()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, wordTrie.count("", possibleID));
    targetWord = wordTrie.getNthWord(dis(gen), possibleID);
}

void Wordle::reset()
{
    guesses = 0;
    status = GameStatus::ONGOING;
    wordlist = cache.wordlistCache;
    auto stat = getStat(0);
    stats.clear();
    stats.push_back(stat);
}

bool Wordle::Word::operator<(const Word &other) const
{
    if (feq(maxEntropy, other.maxEntropy)) return entropy < other.entropy;
    return maxEntropy < other.maxEntropy;
}