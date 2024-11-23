#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include "wordle.h"

using namespace std;
const string allowedFilepath = "res/3b1b/allowed_words.txt";
const string possibleFilepath = "res/3b1b/possible_words.txt";
const string cacheFilepath = "entropy_cache.txt";

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    Wordle wordle(allowedFilepath, possibleFilepath, cacheFilepath);
    auto stat = wordle.getStat(-1);
    stat.print();
    wordle.printTopNWords(10);

    while (!wordle.isGameOver())
    {
        string guess;
        cout << "\nEnter your guess: ";
        cin >> guess;
        cin.ignore();
        cin.clear();

        for (auto &c : guess) c = tolower(c);

        if (!wordle.isWordValid(guess))
        {
            cout << "Invalid word!" << endl;
            continue;
        }
        stat = wordle.guess(guess);
        stat.print();
        if (stat.count <= 50) wordle.printPossibleWords();
        wordle.printTopNWords(10);
    }

    switch (wordle.getStatus())
    {
        case Wordle::GameStatus::WON:
            cout << "Congratulations! You won!" << endl;
            break;
        case Wordle::GameStatus::LOST:
            cout << "You lost! The word was: " << wordle.getTargetWord()
                 << endl;
            break;
        default:
            break;
    }
}