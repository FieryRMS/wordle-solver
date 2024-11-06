---
marp: true
theme: default
paginate: true

---

# Application of Information Theory in Wordle
## Status Report 1, 7/11/2024
### Wong Pak Kiu, Raiyan Mohammed Shah

---

# Overview
Explore how probability and information theory can be applied to efficiently solve a word guessing game known as Wordle.

Topics of interest:
- Entropy
- Surprisal and mutual information

---

<!-- Maybe give a demo on the game? -->

# Current Progress
Implemented the Wordle game in C++, using original rules:
- A maximum of 6 guesses
- Target is a five letter word
- Three kinds of feedback is given to the player:
    1. Green: The letter guessed is in the right place.
    2. Yellow: The letter exists in the answer, but is not in the current place.
    3. Grey: The letter does not exist in the answer.

One exception is, unlike the original Wordle (on NYTimes), the word is *randomly*
chosen from the word list for each game.

---

# Future Work
- Implement formulas to compute entropy in real time.
- Test the solution's effectiveness against other sample sets, i.e. increasing the length of words and changing the words that will appear in the game.
- Modify the probability model on the words to see if the theoretical optimal solution still works.
- Apply the same technique on other Wordle variants, such as **Absurdle** or **Mastermind**

*Note: Absurdle is a variant of Wordle, but there is no definitive answer to the game. According to the author, it "reveals as little information as possible", which*
*probably implies that it uses information theory to stifle guess attempts.*

