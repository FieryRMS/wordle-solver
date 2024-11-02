#include "wordle.h"
#include <fstream>
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
      query(wordlist.query(""))
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filepath << endl;
        exit(1);
    }

    string word;
    while (file >> word) wordlist.insert(word);
    file.close();
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

vector<Wordle::TileType> Wordle::guess(const string &guess)
{
    if (isGameOver()) return {};

    vector<TileType> result(N, TileType::NONE);
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
    updateQuery(guess, result);
    // query.print(); // for debugging
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

void Wordle::updateQuery(const string &guess, const vector<TileType> &result)
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
            case TileType::NONE:
                query.exclude(guess[i]);
                break;
        }
    }
    query.include(includes);
}

int Wordle::count(vector<string> *result)
{
    return wordlist.count(query, result);
}