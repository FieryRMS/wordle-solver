#include "Simulator.h"
#include <fstream>
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

void Simulator::run()
{
    ProgressBar progressBar(words.size());
    int scores[7] = { 0 };
    double averageScore = 0;
    for (int i = 0; i < words.size(); i++)
    {
        wordle.reset();
        wordle.setTargetWord(words[i]);

        progressBar.update(i);
        while (!wordle.isGameOver())
        {
            string guess = wordle.getTopNWords(1)[0].second;
            wordle.guess(guess);
        }

        int score = wordle.getGuesses();
        if (wordle.getStatus() == Wordle::GameStatus::LOST) score = 7;
        scores[score - 1]++;
        averageScore += score;
    }
    progressBar.finish();

    cout << "Average score: " << averageScore / words.size() << endl;
    cout << "Scores: ";
    for (int i = 0; i < 7; i++) cout << scores[i] << " ";
}
