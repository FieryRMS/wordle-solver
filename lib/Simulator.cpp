#include "Simulator.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include "ProgressBar.h"

Simulator::Simulator(const string &filepath, Wordle &wordle) : wordle(wordle)
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filepath << endl;
        exit(1);
    }

    string word;
    while (file >> word) words.push_back(word);
}

void Simulator::run(int n)
{
    ProgressBar progressBar(words.size());
    int scores[7] = { 0 };
    double averageScore = 0;
    vector<string> lostWords;
    vector<pair<double, int>> points;
    for (int i = 0; i < words.size(); i++)
    {
        wordle.reset();
        wordle.setTargetWord(words[i]);

        progressBar.update(i);
        vector<double> remainingBits;
        remainingBits.push_back(wordle.getStat(-1).remainingBits);
        while (!wordle.isGameOver())
        {
            auto guess = wordle.getTopNWords(n)[0];
            auto stat = wordle.guess(guess.word);
            remainingBits.push_back(stat.remainingBits);
        }
        remainingBits.pop_back();

        int score = wordle.getGuesses();
        if (wordle.getStatus() == Wordle::GameStatus::LOST)
        {
            score = 7;
            lostWords.push_back(words[i]);
        }
        scores[score - 1]++;
        averageScore += score;
        for (auto &remainingBit : remainingBits)
        {
            points.push_back({ remainingBit, score-- });
            if (points.back().first == 0 && points.back().second > 1)
            {
                // algo took more than 1 guess to solve when there was no remaining entropy (ie. it could have solved it in 1 guess)
                cout << "WARN: BAD GUESSES FOR WORD: " << words[i] << endl;
            }
        }
    }
    progressBar.finish();

    cout << "Average score: " << averageScore / words.size() << endl;
    cout << "Scores: ";
    for (int i = 0; i < 7; i++) cout << scores[i] << " ";
    cout << endl;
    if (!lostWords.empty())
    {
        cout << "Lost words: ";
        for (auto &word : lostWords) cout << word << " ";
        cout << endl;
    }

    ofstream file("points.txt", ios::trunc);
    for (auto &point : points)
        file << setprecision(18) << point.first << "," << point.second << endl;
}
