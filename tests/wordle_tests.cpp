#include <gtest/gtest.h>
#include <fstream>
#include "trie.h"
#include "wordle.h"

const string filepath = "res/wordle/words";

TEST(WORDLE, VALID_WORD)
{
    Wordle wordle;
    EXPECT_EQ(wordle.getGuesses(), 0);
    EXPECT_EQ(wordle.getMaxGuesses(), 6);
    EXPECT_EQ(wordle.getStatus(), Wordle::GameStatus::ONGOING);

    EXPECT_TRUE(wordle.isWordValid("hello"));
    EXPECT_TRUE(wordle.isWordValid("world"));
    EXPECT_TRUE(wordle.isWordValid("aahed"));
    EXPECT_FALSE(wordle.isWordValid("worlds"));
    EXPECT_FALSE(wordle.isWordValid("12345"));
    EXPECT_FALSE(wordle.isWordValid(""));
    // returns one item but contains .
    EXPECT_FALSE(wordle.isWordValid("...ij"));
}

TEST(WORDLE, GAME_FAIL)
{
    Wordle wordle;

    for (int i = 0; i < 6; i++) wordle.guess("world");

    EXPECT_EQ(wordle.getGuesses(), 6);
    EXPECT_EQ(wordle.getStatus(), Wordle::GameStatus::LOST);
}

TEST(WORDLE, GAME_WIN)
{
    Wordle wordle("hello");

    wordle.guess("hello");

    EXPECT_EQ(wordle.getGuesses(), 1);
    EXPECT_EQ(wordle.getStatus(), Wordle::GameStatus::WON);
}

TEST(WORDLE, GAME_EDGE_CASES)
{
    Wordle wordle("aahed");
    auto result = wordle.guess("bruja"), expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::NONE, Wordle::TileType::NONE,
        Wordle::TileType::NONE, Wordle::TileType::MISPLACED
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("kiaat"), expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::NONE,
        Wordle::TileType::MISPLACED, Wordle::TileType::MISPLACED,
        Wordle::TileType::NONE
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("mahal"), expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::CORRECT,
        Wordle::TileType::CORRECT, Wordle::TileType::MISPLACED,
        Wordle::TileType::NONE
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("shahs"), expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::MISPLACED,
        Wordle::TileType::MISPLACED, Wordle::TileType::NONE,
        Wordle::TileType::NONE
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("bbaaa"), expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::NONE,
        Wordle::TileType::MISPLACED, Wordle::TileType::MISPLACED,
        Wordle::TileType::NONE
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("aahed"), expected = vector<Wordle::TileType>{
        Wordle::TileType::CORRECT, Wordle::TileType::CORRECT,
        Wordle::TileType::CORRECT, Wordle::TileType::CORRECT,
        Wordle::TileType::CORRECT
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);
}

TEST(TRIE, COUNT)
{
    ifstream file(filepath);
    ASSERT_TRUE(file.is_open());

    Trie<5> trie;
    string word;
    while (file >> word) trie.insert(word);

    EXPECT_EQ(trie.count(""), 14855);
    EXPECT_EQ(trie.count(trie.query("")), 14855);

    EXPECT_EQ(trie.count("hello"), 1);
    EXPECT_EQ(trie.count("world"), 1);
    EXPECT_EQ(trie.count("aahed"), 1);

    EXPECT_EQ(trie.count("worlds"), 0);
    EXPECT_DEATH(trie.count("12345"), ".*must be lower case letter");

    EXPECT_EQ(trie.count(trie.query(".z.a.")), 1);
    EXPECT_EQ(trie.count(trie.query("..a.y")), 97);
    EXPECT_EQ(trie.count(trie.query("cur..")), 25);

    auto query1 = trie.query(".a.a.");
    EXPECT_EQ(trie.count(query1), 278);

    query1.include("b");
    EXPECT_EQ(trie.count(query1), 25);

    query1.setMisplaced("b", 0);
    EXPECT_EQ(trie.count(query1), 12);

    query1.exclude("s");
    EXPECT_EQ(trie.count(query1), 8);

    auto query2 = trie.query(".e...");
    EXPECT_EQ(trie.count(query2), 1857);

    query2.include("at");
    EXPECT_EQ(trie.count(query2), 110);

    query2.setMisplaced("sa", 4);
    EXPECT_EQ(trie.count(query2), 53);

    query2.exclude("id");
    EXPECT_EQ(trie.count(query2), 45);
}

TEST(TRIE, NTH_WORD)
{
    ifstream file(filepath);
    ASSERT_TRUE(file.is_open());

    Trie<5> trie;
    string word;
    while (file >> word) trie.insert(word);

    EXPECT_EQ(trie.getNthWord(0), "");
    EXPECT_EQ(trie.getNthWord(1), "aahed");
    EXPECT_EQ(trie.getNthWord(2), "aalii");
    EXPECT_EQ(trie.getNthWord(5956), "jaded");
    EXPECT_EQ(trie.getNthWord(6969), "litas");
    EXPECT_EQ(trie.getNthWord(7978), "motes");
    EXPECT_EQ(trie.getNthWord(8914), "othyl");
    EXPECT_EQ(trie.getNthWord(10007), "pugil");
    EXPECT_EQ(trie.getNthWord(12345), "state");
    EXPECT_EQ(trie.getNthWord(14855), "zymic");
}