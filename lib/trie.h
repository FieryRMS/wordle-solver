#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>


using namespace std;

template <size_t N>
class Trie {
   public:
    class Query {
       public:
        void parse(const string &s);
        void setCorrect(const char &c, const int &idx);
        void include(const string &s);
        void include(const char &c, const int count = 1);
        void exclude(const string &s);
        void exclude(const char &c);
        void setMisplaced(const string &s, const int &idx);
        void setMisplaced(const char &c, const int &idx);
        void print() const;
        bool verify(const string &word);

       private:
        Query(const string &s);
        int includes[26];
        int includesCount;
        bool misplaced[N][26];
        bool excludes[26];
        char letters[N];
        bool verify(const char &c, const int &idx) const;

        friend class Trie<N>;
    };

    Trie();
    void insert(const string &word);
    int count(Query query, vector<string> *result = nullptr) const;
    int count(const string &word) const;
    map<string, int> getPatternsCounts(const string &word,
                                       Query &SampleSpace) const;
    string getNthWord(int n) const;
    Query query(const string s = "") const;

    ~Trie();

   private:
    struct Node {
        Node *children[26];
        int count;
        bool isEnd;
        Node();

        ~Node();
    };

    // including from Wordle.h causes circular dependency, so I just copy paste ¯\_(ツ)_/¯
    enum TileType {
        CORRECT = 'C',
        WRONG = 'W',
        MISPLACED = 'M',
        NONE = '.',
    };

    Node *root;
    static int index(const char &c);
    int _count(Query &query,
               Node *node,
               vector<string> *result,
               string *word = nullptr,
               const string *guess = nullptr,
               set<int> (*guessLetters)[26] = nullptr,
               map<string, int> *memo = nullptr,
               string *pattern = nullptr,
               int idx = 0) const;
};

#include "trie.tpp"  // IWYU pragma: keep