#pragma once
#include <string>

using namespace std;

class Trie {
   public:
    Trie();
    void insert(const string &word);
    int count(const string &word) const;
    string getNthWord(int n) const;

    ~Trie();

   private:
    struct Node {
        Node *children[27];
        int count;
        bool isEnd;
        Node();

        ~Node();
    };

    Node *root;
    int index(const char &c) const;
    void _insert(Node *node, string word);
};