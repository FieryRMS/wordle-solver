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
        int cnt = wordle.count();
        cout << "REMAINING POSSIBILITIES: " << cnt << endl;
        if (cnt <= 50)
        {
            vector<string> result;
            wordle.count(&result);
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

        cout << wordle.guess2emoji(wordle.guess(guess)) << endl;
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