#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include "wordle.h"

using namespace std;

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    Wordle wordle;

    while (!wordle.isGameOver())
    {
        cout << "GUESS " << wordle.getGuesses() + 1 << "/"
             << wordle.getMaxGuesses() << endl;
        // cout << "TARGET WORD: " << wordle.getTargetWord() << endl;
        auto stat = wordle.getStat(-1);
        cout << "REMAINING WORDS:       " << stat.count << endl
             << "INFORMATION GAINED:    " << fixed << setprecision(2) << setw(5)
             << stat.bits << " bits" << endl
             << "REMAINING INFORMATION: " << fixed << setprecision(2) << setw(5)
             << stat.remainingBits << " bits" << endl;
        if (stat.count <= 50)
        {
            vector<string> result = wordle.getWords(-1);
            cout << "POSSIBILITIES: {";
            for (auto &word : result) cout << '"' << word << "\", ";
            cout << "}" << endl;
        }
        string guess;
        cout << "Enter your guess: ";
        cin >> guess;

        for (auto &c : guess) c = tolower(c);

        if (!wordle.isWordValid(guess))
        {
            cout << "Invalid word!" << endl;
            continue;
        }

        cout << wordle.guess2emoji(wordle.guess(guess).result) << endl;
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