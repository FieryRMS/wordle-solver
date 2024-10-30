#include "trie.h"
#include <cassert>
using namespace std;

Trie::Trie()
{
    root = new Node();
}

Trie::Node::Node() : children(), count(0) {}

Trie::~Trie()
{
    delete root;
}

Trie::Node::~Node()
{
    for (auto &child : children)
        if (child) delete child;
}

int Trie::index(const char &c) const
{
    if (c == '.') return 26;
    assert(islower(c) && "must be lower case letter or .");
    return c - 'a';
}

void Trie::insert(const string &word)
{
    _insert(root, word);
}

void Trie::_insert(Node *node, string word)
{
    node->count++;
    if (word.empty()) return;

    int i = index(word[0]);
    if (!node->children[i]) node->children[i] = new Node();
    _insert(node->children[i], word.substr(1));

    // insert for wildcard
    i = index('.');
    if (!node->children[i]) node->children[i] = new Node();
    _insert(node->children[i], word.substr(1));
}

int Trie::count(const string &word) const
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

string Trie::getNthWord(int n) const
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
                flag = true;
                break;
            }
            n -= node->children[i]->count;
        }
        if (!flag) break;
    }
    return word;
}