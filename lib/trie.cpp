#include "trie.h"
#include <cassert>
#include <iostream>

using namespace std;

template <size_t N>
Trie<N>::Trie()
{
    root = new Node();
}

template <size_t N>
Trie<N>::Node::Node()
    : children(), count(), letterCntAtPos(), WordCountWithLetter(), isEnd(false)
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
 * @param id
 */
template <size_t N>
void Trie<N>::insert(const string &word, const ID &id)
{
    assert(word.size() == N && "invalid word size");

    Node *node = root;
    node->count[id]++;
    for (int i = 0; i < N; i++)
    {
        int occurences[26] = { 0 };
        bool seen[26] = { 0 };
        for (int j = 0; j < N; j++)
        {
            node->letterCntAtPos[id][j][index(word[j])]++;
            if (j >= i && !seen[index(word[j])])
            {
                node->WordCountWithLetter[id][index(word[j])]++;
                seen[index(word[j])] = true;
            }

            occurences[index(word[j])]++;
            node->letterOccuredAtleast[id][index(word[j])]
                                      [occurences[index(word[j])]]++;
        }

        if (!node->children[index(word[i])])
            node->children[index(word[i])] = new Node();
        node = node->children[index(word[i])];
        node->count[id]++;
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
int Trie<N>::count(const string &word, const ID &id) const
{
    Node *node = root;
    for (auto &c : word)
    {
        int i = index(c);
        if (!node->children[i] || node->children[i]->count[id] == 0) return 0;
        node = node->children[i];
    }
    return node->count[id];
}

/**
 * @brief Alphabetically get the nth word in the trie
 *
 * @tparam N
 * @param n
 * @return string
 */
template <size_t N>
string Trie<N>::getNthWord(int n, const ID &id) const
{
    string word = "";
    Node *node = root;
    while (n > 0)
    {
        bool flag = false;
        for (int i = 0; i < 26; i++)
        {
            if (!node->children[i] || node->children[i]->count[id] == 0)
                continue;
            if (n <= node->children[i]->count[id])
            {
                word += 'a' + i;
                node = node->children[i];
                if (node->isEnd) n--;  // should be 0 at this point
                flag = true;
                break;
            }
            n -= node->children[i]->count[id];
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
Trie<N>::Query Trie<N>::query(const string s, const ID &id) const
{
    return Query(s, id);
}

template <size_t N>
Trie<N>::Query::Query(const string &s, const ID &id)
    : trieId(id),
      includes(),
      includesCount(0),
      misplaced(),
      excludes(),
      letters()
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
    string word(N, '.');
    int calls = 0;
    return _count(query, root, word, calls, result);
}
template <size_t N>
int Trie<N>::_count(
    Query &query,
    Node *node,
    string &word,
    int &calls,

    // provide below params if you want to store the words
    vector<string> *result,

    // provide the below params if you want to calculate patterns
    const string *guess,
    set<int> (*guessLetters)[26],
    unordered_map<string, int> *memo,
    string *pattern,

    int idx) const
{
    calls++;
    if (idx == N)
    {
        if (result) result->push_back(word);
        if (memo && pattern) (*memo)[*pattern]++;
        return node->count[query.trieId];  // or node->isEnd?
    }

    // not enough letters left
    if (query.includesCount > N - idx) return 0;

    for (int i = idx; i < 5; i++)
    {
        // fixed letter, but no letter exists in subtree
        if (query.letters[i] &&
            node->letterCntAtPos[query.trieId][i][index(query.letters[i])] == 0)
            return 0;
        // misplaced letter, but all words have it at that position
        for (int j = 0; j < 26; j++)
        {
            if (query.misplaced[i][j] &&
                node->letterCntAtPos[query.trieId][i][j] ==
                    node->count[query.trieId])
                return 0;

            // includes letter, but all the words that do have it, have it in misplaced
            if (query.includes[j] && query.misplaced[i][j] &&
                node->letterCntAtPos[query.trieId][i][j] ==
                    node->WordCountWithLetter[query.trieId][j])
                return 0;
        }
    }
    for (int i = 0; i < 26; i++)
    {
        // includes letter, but subtree does not have enough
        if (query.includes[i] &&
            !node->letterOccuredAtleast[query.trieId][i][query.includes[i]])
            return 0;
        // excludes letter, but all of subtree has it
        if (query.excludes[i] && !query.includes[i] &&
            node->WordCountWithLetter[query.trieId][i] ==
                node->count[query.trieId])
            return 0;
    }

    int sum = 0;
    bool flag = false;

    for (int i = 0; i < 26; i++)
    {
        if (!node->children[i] || node->children[i]->count[query.trieId] == 0)
            continue;
        if (!query.verify('a' + i, idx)) continue;

        // prepare to traverse the next node
        if (query.includes[i])
            query.includes[i]--, query.includesCount--, flag = true;
        word[idx] = 'a' + i;

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
        sum += _count(query, node->children[i], word, calls, result, guess,
                      guessLetters, memo, pattern, idx + 1);

        // undo the changes
        word[idx] = '.';
        if (flag) query.includes[i]++, query.includesCount++, flag = false;

        if (removedIdx != -1 && prevMissPattern != -1)
            (*pattern)[removedIdx] = prevMissPattern;
        if (removedIdx != -1) (*guessLetters)[i].insert(removedIdx);
        if (prevPattern != -1) (*pattern)[idx] = prevPattern;
    }

    // if (sum == 0) { cout << "WORD: " << word << endl; }
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
unordered_map<string, int> Trie<N>::getPatternsCounts(const string &guess,
                                            Query &SampleSpace) const
{
    unordered_map<string, int> memo;
    string pattern(N, TileType::NONE);
    set<int> guessLetters[26];
    string word(N, '.');
    int calls = 0;

    for (int i = N - 1; i >= 0; i--) guessLetters[index(guess[i])].insert(i);

    _count(SampleSpace, root, word, calls, nullptr, &guess, &guessLetters,
           &memo, &pattern);
    // cout << "Calls: " << calls << endl;
    return memo;
}

template <size_t N>
bool Trie<N>::Query::verify(const char &c, const int &idx) const
{
    if (excludes[index(c)] && includes[index(c)] == 0)
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

template <size_t N>
string Trie<N>::Query::serialize() const
{
    string result = "";

    // letters
    for (int i = 0; i < N; i++)
    {
        if (letters[i]) result += letters[i];
        else result += '.';
    }

    result += delim;
    // includes
    for (int i = 0; i < 26; i++)
        if (includes[i]) result += 'a' + i, result += to_string(includes[i]);

    result += delim;
    // excludes
    for (int i = 0; i < 26; i++)
        if (excludes[i]) result += 'a' + i;

    result += delim;
    // misplaced
    for (int i = 0; i < N; i++)
    {
        result += to_string(i);
        for (int j = 0; j < 26; j++)
            if (misplaced[i][j]) result += 'a' + j;
    }

    return result;
}

template class Trie<5>;