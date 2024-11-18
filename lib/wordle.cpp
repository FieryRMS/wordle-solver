#include "wordle.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

using namespace std;

const string filepath = "res/wordle/words";

Wordle::Wordle() : Wordle("")
{
    // choose a random word from the list
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, wordlist.count(""));
    targetWord = wordlist.getNthWord(dis(gen));
}

Wordle::Wordle(const string &targetWord)
    : targetWord(targetWord),
      guesses(0),
      status(GameStatus::ONGOING),
      wordlist()
{
    stats.reserve(maxGuesses + 1);
    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filepath << endl;
        exit(1);
    }

    string word;
    while (file >> word) wordlist.insert(word);
    file.close();

    int count = wordlist.count("");

    stats.push_back({
        .guess = "",
        .result = {},
        .count = count,
        .patternProb = 0,
        .bits = 0,
        .expectedBits = 0,
        .remainingBits = log2(count),
        .query = wordlist.query(""),
        .valid = true,
    });
}

bool Wordle::isWordValid(const string &word)
{
    if (word.size() != N) return false;

    for (auto &c : word)
        if (!islower(c)) return false;

    // if not in wordlist return false
    if (wordlist.count(word) == 1) return true;
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
            .query = wordlist.query(""),
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
    int count = wordlist.count(query), prevCount = stats.back().count;

    // Information = log2(1 / P(x)) = - log2(P(x)) = - log2(count / prevCount) = log2(prevCount) - log2(count)
    double bits = log2(prevCount) - log2(count);

    stats.push_back({
        .guess = guess,
        .result = result,
        .count = count,
        .patternProb = (double)count / prevCount,
        .bits = bits,
        .expectedBits = getExpectedBits(-1, guess),
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
    wordlist.count(getStat(i).query, &result);
    return result;
}

double Wordle::getExpectedBits(int i, string guess) const
{
    auto stat = getStat(i);
    auto patterns = wordlist.getPatternsCounts(guess, stat.query);
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
    const int col1 = 23, col2 = 5;

    cout << setw(col1) << "GUESS: " << guess << endl;
    cout << setw(col1) << "PATTERN: " << guess2emoji(result) << endl;
    cout << setw(col1) << "REMAINING WORDS: " << setw(col2) << count << endl;
    cout << setw(col1) << "PATTERN PROBABILITY: " << setw(col2) << fixed
         << setprecision(2) << patternProb << endl;
    cout << setw(col1) << "INFORMATION GAINED: " << setw(col2) << fixed
         << setprecision(2) << bits << " bits" << endl;
    cout << setw(col1) << "EXPECTED GAIN: " << setw(col2) << fixed
         << setprecision(2) << expectedBits << " bits" << endl;
    cout << setw(col1) << "REMAINING INFORMATION: " << setw(col2) << fixed
         << setprecision(2) << remainingBits << " bits" << endl;
}