#pragma once
#include <cassert>
#include <iostream>
#include "trie.h"

using namespace std;

template <size_t N>
Trie<N>::Trie()
{
    root = new Node();
}

template <size_t N>
Trie<N>::Node::Node() : children(), count(0), isEnd(false)
{}

template <size_t N>
Trie<N>::~Trie()
{
    delete root;
}

template <size_t N>
Trie<N>::Node::~Node()
{
    for (auto &child : children)
        if (child) delete child;
}

template <size_t N>
int Trie<N>::index(const char &c)
{
    assert(islower(c) && "must be lower case letter");
    return c - 'a';
}

/**
 * @brief Insert a word into the trie
 *
 * @tparam N
 * @param word
 */
template <size_t N>
void Trie<N>::insert(const string &word)
{
    assert(word.size() == N && "invalid word size");

    Node *node = root;
    node->count++;
    for (auto &c : word)
    {
        int i = index(c);
        if (!node->children[i]) node->children[i] = new Node();
        node = node->children[i];
        node->count++;
    }
    node->isEnd = true;
}

/**
 * @brief Get number of words in the trie that match the string prefix
 *
 * @tparam N
 * @param word
 * @return int
 */
template <size_t N>
int Trie<N>::count(const string &word) const
{
    Node *node = root;
    for (auto &c : word)
    {
        int i = index(c);
        if (!node->children[i]) return 0;
        node = node->children[i];
    }
    return node->count;
}

/**
 * @brief Alphabetically get the nth word in the trie
 *
 * @tparam N
 * @param n
 * @return string
 */
template <size_t N>
string Trie<N>::getNthWord(int n) const
{
    string word = "";
    Node *node = root;
    while (n > 0)
    {
        bool flag = false;
        for (int i = 0; i < 26; i++)
        {
            if (!node->children[i]) continue;
            if (n <= node->children[i]->count)
            {
                word += 'a' + i;
                node = node->children[i];
                if (node->isEnd) n--;  // should be 0 at this point
                flag = true;
                break;
            }
            n -= node->children[i]->count;
        }
        if (!flag) break;
    }
    return word;
}

/**
 * @brief Create a query object from a string
 *
 * @tparam N
 * @param s
 * @return Trie<N>::Query
 */
template <size_t N>
Trie<N>::Query Trie<N>::query(const string s) const
{
    return Query(s);
}

template <size_t N>
Trie<N>::Query::Query(const string &s)
    : includes(), excludes(), letters(), misplaced(), includesCount(0)
{
    parse(s);
}

/**
 * @brief fix the letters in the query
 *
 * @tparam N
 * @param s
 */
template <size_t N>
void Trie<N>::Query::parse(const string &s)
{
    assert(s.size() <= N && "invalid query size");
    for (int i = 0; i < s.length(); i++) setCorrect(s[i], i);
}

/**
 * @brief Set the correct letter at the given index
 *
 * @tparam N
 * @param c
 * @param idx
 */
template <size_t N>
void Trie<N>::Query::setCorrect(const char &c, const int &idx)
{
    letters[idx] = islower(c) ? c : 0;
}

/**
 * @brief Letters that must be included in the word
 *
 * @tparam N
 * @param s
 */
template <size_t N>
void Trie<N>::Query::include(const string &s)
{
    int counts[26] = { 0 };
    for (auto &c : s) counts[index(c)]++;
    for (int i = 0; i < 26; i++)
        if (counts[i]) include('a' + i, counts[i]);
}

/**
 * @brief Include a letter in the word
 *
 * @tparam N
 * @param c
 */
template <size_t N>
void Trie<N>::Query::include(const char &c, const int count)
{
    if (count > includes[index(c)])
        includesCount += count - includes[index(c)], includes[index(c)] = count;
}

/**
 * @brief Letters that must not be included in the word
 *
 * @tparam N
 * @param s
 */
template <size_t N>
void Trie<N>::Query::exclude(const string &s)
{
    for (auto &c : s) exclude(c);
}

/**
 * @brief Exclude a letter from the word
 *
 * @tparam N
 * @param c
 */
template <size_t N>
void Trie<N>::Query::exclude(const char &c)
{
    excludes[index(c)] = true;
}

/**
 * @brief Letters that should not appear in the given index
 *
 * @tparam N
 * @param s
 * @param idx
 */
template <size_t N>
void Trie<N>::Query::setMisplaced(const string &s, const int &idx)
{
    for (auto &c : s) setMisplaced(c, idx);
}

/**
 * @brief Set a letter that should not appear in the given index
 *
 * @tparam N
 * @param c
 * @param idx
 */
template <size_t N>
void Trie<N>::Query::setMisplaced(const char &c, const int &idx)
{
    misplaced[idx][index(c)] = true;
}

/**
 * @brief Count the number of words that match the query
 *
 * @tparam N
 * @param query
 * @param result provide if you want to store the words
 * @return int
 */
template <size_t N>
int Trie<N>::count(Query query, vector<string> *result) const
{
    string *word = nullptr;
    if (result) word = new string(N, '.');
    return _count(query, root, result, word);
}

template <size_t N>
int Trie<N>::_count(
    Query &query,
    Node *node,

    // provide below params if you want to store the words
    vector<string> *result,
    string *word,

    // provide the below params if you want to calculate patterns
    const string *guess,
    set<int> (*guessLetters)[26],
    map<string, int> *memo,
    string *pattern,

    int idx) const
{
    if (idx == N)
    {
        if (result && word) result->push_back(*word);
        if (memo && pattern) (*memo)[*pattern]++;
        return node->count;  // or node->isEnd?
    }

    // not enough letters left
    if (query.includesCount > N - idx) return 0;

    int sum = 0;
    bool flag = false;

    for (int i = 0; i < 26; i++)
    {
        if (!node->children[i]) continue;
        if (!query.test('a' + i, idx)) continue;

        // prepare to traverse the next node
        if (query.includes[i])
            query.includes[i]--, query.includesCount--, flag = true;
        if (word) (*word)[idx] = 'a' + i;

        int removedIdx = -1;
        char prevPattern = -1, prevMissPattern = -1;
        // do we need to check the pattern?
        if (guess && guessLetters && pattern)
        {
            prevPattern = (*pattern)[idx];
            bool checkMissplaced = false;
            // if the current letter is the same, correct
            if ((*guess)[idx] == 'a' + i)
            {
                // remove from guessLetters if never assigned
                // we remove it so that the misplaced logic doesnt overwrite it
                if ((*pattern)[idx] == TileType::NONE)
                {
                    removedIdx = idx;
                    (*guessLetters)[i].erase(idx);
                }
                // it was previously assigned by misplaced, so we need to check again
                else checkMissplaced = true;

                (*pattern)[idx] = TileType::CORRECT;
            }
            else
            {
                // if never assigned, mark it as wrong
                if ((*pattern)[idx] == TileType::NONE)
                    (*pattern)[idx] = TileType::WRONG;
                // else ignore, it was previously assigned by misplaced
                // check if the current word letter is misplaced
                checkMissplaced = true;
            }

            // is the letter in the guess?
            if (checkMissplaced && !(*guessLetters)[i].empty())
            {
                int missIdx = *(*guessLetters)[i].begin();
                removedIdx = missIdx;
                prevMissPattern = (*pattern)[missIdx];
                (*pattern)[missIdx] = TileType::MISPLACED;
                (*guessLetters)[i].erase(missIdx);
            }
        }

        // traverse the next node
        sum += _count(query, node->children[i], result, word, guess,
                      guessLetters, memo, pattern, idx + 1);

        // undo the changes
        if (word) (*word)[idx] = '.';
        if (flag) query.includes[i]++, query.includesCount++, flag = false;

        if (removedIdx != -1 && prevMissPattern != -1)
            (*pattern)[removedIdx] = prevMissPattern;
        if (removedIdx != -1) (*guessLetters)[i].insert(removedIdx);
        if (prevPattern != -1) (*pattern)[idx] = prevPattern;
    }

    return sum;
}
/**
 * @brief Print the query object
 *
 * @tparam N
 */
template <size_t N>
void Trie<N>::Query::print() const
{
    cout << "QUERY: ";
    for (int i = 0; i < N; i++)
    {
        if (letters[i]) cout << letters[i];
        else cout << '.';
    }
    cout << endl;
    cout << "INCLUDES: ";
    for (int i = 0; i < 26; i++)
        if (includes[i]) cout << (char)('a' + i) << "=" << includes[i] << ", ";
    cout << endl;
    cout << "EXCLUDES: ";
    for (int i = 0; i < 26; i++)
        if (excludes[i]) cout << (char)('a' + i);
    cout << endl;
    cout << "MISPLACED: " << endl;
    for (int i = 0; i < N; i++)
    {
        cout << i << ": ";
        for (int j = 0; j < 26; j++)
            if (misplaced[i][j]) cout << (char)('a' + j) << ", ";
        cout << endl;
    }
}

template <size_t N>
map<string, int> Trie<N>::getPatternsCounts(const string &guess,
                                            Query &SampleSpace) const
{
    map<string, int> memo;
    string pattern(N, TileType::NONE);
    set<int> guessLetters[26];
    string word(N, '.');

    for (int i = N - 1; i >= 0; i--) guessLetters[index(guess[i])].insert(i);

    _count(SampleSpace, root, nullptr, &word, &guess, &guessLetters, &memo,
           &pattern);
    return memo;
}

template <size_t N>
bool Trie<N>::Query::verify(const char &c, const int &idx) const
{
    if (excludes[index(c)] && includes[index(c)] == 0 && !letters[idx])
        return false;
    if (misplaced[idx][index(c)]) return false;
    if (letters[idx] && letters[idx] != c) return false;
    // includesCount left, N - idx steps left but current letter not included
    if (includesCount == N - idx && includes[index(c)] == 0) return false;

    return true;
}

template <size_t N>
bool Trie<N>::Query::verify(const string &word)
{
    vector<int> included;
    included.reserve(5);
    bool flag = true;
    for (int i = 0; i < N; i++)
    {
        if (!verify(word[i], i))
        {
            flag = false;
            break;
        }
        if (includes[index(word[i])])
            includes[index(word[i])]--, includesCount--,
                included.push_back(index(word[i]));
    }
    for (auto &i : included) includes[i]++, includesCount++;
    return flag;
}