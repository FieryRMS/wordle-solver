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

template <size_t N>
int Trie<N>::count(Query query, vector<string> *result) const
{
    string *word = nullptr;
    if (result) word = new string(), word->reserve(N);
    return _count(query, root, result, word);
}

template <size_t N>
int Trie<N>::_count(Query &query,
                    Node *node,
                    vector<string> *result,
                    string *word,
                    int idx) const
{
    if (idx == N)
    {
        if (result && word) result->push_back(*word);
        return node->count;  // or node->isEnd?
    }

    // not enough letters left
    if (query.includesCount > N - idx) return 0;

    int sum = 0;
    bool flag = false;

    for (int i = 0; i < 26; i++)
    {
        if (!node->children[i]) continue;
        if (query.excludes[i] && query.includes[i] == 0) continue;
        if (query.misplaced[idx][i]) continue;
        if (query.letters[idx] && query.letters[idx] != 'a' + i) continue;
        // includesCount left, N - idx steps left but current letter not
        // included
        if (query.includesCount == N - idx && query.includes[i] == 0) continue;
        if (query.includes[i])
            query.includes[i]--, query.includesCount--, flag = true;
        if (word) word->push_back('a' + i);
        sum += _count(query, node->children[i], result, word, idx + 1);
        if (word) word->pop_back();
        if (flag) query.includes[i]++, query.includesCount++, flag = false;
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