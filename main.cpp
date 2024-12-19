#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include "Simulator.h"
#include "wordle.h"
#include "wordleLoop.h"

using namespace std;
const string allowedFilepath = "res/3b1b/allowed_words.txt";
const string possibleFilepath = "res/3b1b/possible_words.txt";
const string cacheFilepath = "entropy_cache.txt";

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    WordleLoop wordle(allowedFilepath, possibleFilepath, cacheFilepath);
    Simulator sim(possibleFilepath, wordle);

    char choice;
    cin >> choice;
    cout << "Run simulator? (y/n): ";
    if (choice == 'y')
    {
        sim.run(100);
        wordle.saveCache();
        wordle.reset();
        wordle.setRandomTargetWord();
    }

    cout << "Starting game..." << endl;

    while (true)
    {
        // wordle.setTargetWord("clued");
        auto stat = wordle.getStat(-1);
        while (true)
        {
            stat.print();
            if (stat.count <= 50) wordle.printPossibleWords();
            wordle.printTopNWords(10);

            // stat.query.print();

            if (wordle.isGameOver()) break;

            string guess;
            cout << "\nEnter guess " << wordle.getGuesses() + 1 << "/"
                 << wordle.getMaxGuesses() << ": ";
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

        cout << "Resetting game..." << endl << endl;
        wordle.savePatternCache();
        wordle.reset();
        wordle.setRandomTargetWord();
    }
}
