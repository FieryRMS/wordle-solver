#pragma once
#include <cstddef>
#include <string>

using namespace std;
template <size_t N>
class Trie {
   public:
    class Query {
       public:
        void parse(const string &s);
        void include(const string &s);
        void exclude(const string &s);
        void setMisplaced(const string &s, const int &idx);

       private:
        Query(const string &s);
        int includes[26];
        int includesCount;
        bool misplaced[N][26];
        bool excludes[26];
        char letters[N];

        friend class Trie<N>;
    };

    Trie();
    void insert(const string &word);
    int count(Query query) const;
    int count(const string &word) const;
    string getNthWord(int n) const;
    Query query(const string &s) const;

    ~Trie();

   private:
    struct Node {
        Node *children[26];
        int count;
        bool isEnd;
        Node();

        ~Node();
    };

    Node *root;
    static int index(const char &c);
    int _count(Query &query, Node *node, int idx = 0) const;
};

#include "trie.tpp" // IWYU pragma: keep