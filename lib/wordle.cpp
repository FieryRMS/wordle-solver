#include "wordle.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

using namespace std;

const int titleWidth = 23, numWidth = 5;
const string filepath = "res/wordle/words";
const string EntropyCache = "entropy_cache.txt";

Wordle::Wordle() : Wordle("")
{
    // choose a random word from the list
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, wordTrie.count(""));
    targetWord = wordTrie.getNthWord(dis(gen));
}

Wordle::Wordle(const string &targetWord)
    : targetWord(targetWord),
      guesses(0),
      status(GameStatus::ONGOING),
      wordTrie()
{
    stats.reserve(maxGuesses + 1);

    // check if cache exists
    ifstream cache_in(EntropyCache);
    if (cache_in.is_open())
    {
        string word;
        double entropy;
        while (cache_in >> word >> entropy)
        {
            wordTrie.insert(word);
            wordlist.push_back({ entropy, word });
        }
        cache_in.close();

        cout << "WARN: Using cached entropy values. To update cache delete "
                "file: "
             << EntropyCache << endl;
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
            wordTrie.insert(word);
            wordlist.push_back({ -1, word });
        }
        file.close();
    }

    int count = wordTrie.count("");

    stats.push_back({
        .guess = "",
        .result = {},
        .count = count,
        .patternProb = 0,
        .bits = 0,
        .expectedBits = 0,
        .remainingBits = log2(count),
        .query = wordTrie.query(""),
        .valid = true,
    });

    getTopNWords(0, true);
    // save cache
    ofstream cache_out(EntropyCache);
    for (auto &word : wordlist)
        cache_out << word.second << " " << setprecision(17) << word.first
                  << endl;
}

bool Wordle::isWordValid(const string &word)
{
    if (word.size() != N) return false;

    for (auto &c : word)
        if (!islower(c)) return false;

    // if not in wordlist return false
    if (wordTrie.count(word) == 1) return true;
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
            .query = wordTrie.query(""),
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
    auto query = getUpdatedQuery(guess, result, stats.back().query);
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
    // So we can skip when we have 10 elements, and the smallest element is > next elements old entropy
    double progress = 0;
    if (showProgress)
    {
        cout << "Pre-calculating entropy..." << endl;
        updateProgressbar(progress);
    }
    set<pair<double, string>> topWords;
    auto word = wordlist.begin();
    for (int i = 0; word != wordlist.end(); word++, i++)

    {
        if (word->first == -1 ||
            (n != 0 &&
             (topWords.size() < n || word->first > topWords.begin()->first)))
        {
            word->first = getEntropy(-1, word->second);  // expensive
            topWords.insert(*word);
            if (topWords.size() > n) topWords.erase(topWords.begin());
        }
        else break;

        if (showProgress)
        {
            progress = (double)i / wordlist.size();
            updateProgressbar(progress);
        }
    }
    // we dont need to sort rest of the words, since already sorted
    sort(wordlist.begin(), word, greater<pair<double, string>>());

    if (showProgress)
    {
        progress = 1;
        updateProgressbar(progress);
        cout << endl;
    }

    return vector<pair<double, string>>(wordlist.begin(), wordlist.begin() + n);
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
    auto topWords = getTopNWords(n);
    string title = "TOP " + to_string(n) + " WORDS: ";
    cout << setw(titleWidth) << title << "WORDS | ENTROPY" << endl;
    for (auto &word : topWords)
        cout << setw(titleWidth) << "" << word.second << " | " << setw(numWidth)
             << fixed << setprecision(2) << word.first << endl;
}

void Wordle::updateProgressbar(double progress) const
{
    const int barWidth = 70;
    cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << int(progress * 100.0) << " %\r";
    cout.flush();
}