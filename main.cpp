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
    auto stat = wordle.getStat(-1);
    stat.print();

    while (!wordle.isGameOver())
    {
        string guess;
        cout << "\nEnter your guess: ";
        cin >> guess;

        for (auto &c : guess) c = tolower(c);

        if (!wordle.isWordValid(guess))
        {
            cout << "Invalid word!" << endl;
            continue;
        }
        stat = wordle.guess(guess);
        stat.print();
        if (stat.count <= 50)
        {
            vector<string> result = wordle.getWords(-1);
            cout << "POSSIBILITIES: {";
            for (auto &word : result) cout << '"' << word << "\", ";
            cout << "}" << endl;
        }
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