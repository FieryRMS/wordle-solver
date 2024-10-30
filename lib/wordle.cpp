#include "wordle.h"
#include <fstream>
#include <iostream>
#include <random>
using namespace std;

const string filepath = "res/wordle/words";

Wordle::Wordle() : guesses(0), status(GameStatus::ONGOING)
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filepath << endl;
        exit(1);
    }

    string word;
    while (file >> word) wordlist.insert(word);

    // choose a random word from the list
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, wordlist.count(""));
    targetWord = wordlist.getNthWord(dis(gen));
}

Wordle::Wordle(const string &targetWord)
    : targetWord(targetWord), guesses(0), status(GameStatus::ONGOING)
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filepath << endl;
        exit(1);
    }

    string word;
    while (file >> word) wordlist.insert(word);
}

bool Wordle::isWordValid(const string &word)
{
    if (word.size() != targetWord.size()) return false;

    // if word contains . return false
    if (word.find('.') != string::npos) return false;

    // if not in wordlist return false
    if (wordlist.count(word) == 1) return true;
    return false;
}

vector<Wordle::TileType> Wordle::guess(const string &guess)
{
    if (isGameOver()) return {};

    vector<TileType> result(targetWord.size(), TileType::NONE);
    vector<bool> visited(targetWord.size(), false);

    // check for correct letters
    int cnt = 0;
    for (int i = 0; i < targetWord.size(); i++)
    {
        if (targetWord[i] == guess[i])
        {
            result[i] = TileType::CORRECT;
            visited[i] = true;
            cnt++;
        }
    }

    // check for misplaced letters
    for (int i = 0; i < guess.size(); i++)
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
    if (cnt == targetWord.size()) status = GameStatus::WON;
    else if (guesses == maxGuesses) status = GameStatus::LOST;

    return result;
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
            case TileType::NONE:
                emojis += "🟥";
                break;
        }
    }
    return emojis;
}