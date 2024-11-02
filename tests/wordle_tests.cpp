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

TEST(WORDLE, COUNT1)
{
    Wordle wordle("thowl");
    EXPECT_EQ(wordle.count(), 14855);

    wordle.guess("slate");
    EXPECT_EQ(wordle.count(), 51);

    vector<string> result1;
    wordle.guess("limit");
    EXPECT_EQ(wordle.count(&result1), 11);
    vector<string> expected1{
        "butyl", "hotly", "octyl", "othyl", "thowl", "thurl",
        "tolly", "tolyl", "troll", "trull", "truly",
    };
    EXPECT_EQ(result1, expected1);

    vector<string> result2;
    wordle.guess("truly");
    EXPECT_EQ(wordle.count(&result2), 1);
    vector<string> expected2{ "thowl" };
    EXPECT_EQ(result2, expected2);

    vector<string> result3;
    wordle.guess("thowl");
    EXPECT_EQ(wordle.count(&result3), 1);
    vector<string> expected3{ "thowl" };
    EXPECT_EQ(result3, expected3);

    EXPECT_TRUE(wordle.isGameOver());
    EXPECT_EQ(wordle.getStatus(), Wordle::GameStatus::WON);
}

TEST(WORDLE, COUNT2)
{
    Wordle wordle("breys");
    EXPECT_EQ(wordle.count(), 14855);

    wordle.guess("warns");
    EXPECT_EQ(wordle.count(), 308);

    vector<string> result1;
    wordle.guess("srsly");
    EXPECT_EQ(wordle.count(&result1), 7);
    vector<string> expected1{
        "breys", "dreys", "greys", "preys", "treys", "troys", "tryps",
    };
    EXPECT_EQ(result1, expected1);

    vector<string> result2;
    wordle.guess("donas");
    EXPECT_EQ(wordle.count(&result2), 5);
    vector<string> expected2{
        "breys", "greys", "preys", "treys", "tryps",
    };
    EXPECT_EQ(result2, expected2);

    vector<string> result3;
    wordle.guess("mynah");
    EXPECT_EQ(wordle.count(&result3), 5);
    vector<string> expected3{
        "breys", "greys", "preys", "treys", "tryps",
    };
    EXPECT_EQ(result3, expected3);

    vector<string> result4;
    wordle.guess("treys");
    EXPECT_EQ(wordle.count(&result4), 3);
    vector<string> expected4{
        "breys",
        "greys",
        "preys",
    };
    EXPECT_EQ(result4, expected4);

    vector<string> result5;
    wordle.guess("breys");
    EXPECT_EQ(wordle.count(&result5), 1);
    vector<string> expected5{ "breys" };
    EXPECT_EQ(result5, expected5);

    EXPECT_TRUE(wordle.isGameOver());
    EXPECT_EQ(wordle.getStatus(), Wordle::GameStatus::WON);
}

TEST(WORDLE, COUNT3)
{
    Wordle wordle("bribe");
    EXPECT_EQ(wordle.count(), 14855);

    wordle.guess("query");
    EXPECT_EQ(wordle.count(), 1223);

    wordle.guess("twier");
    EXPECT_EQ(wordle.count(), 45);

    vector<string> result1;
    wordle.guess("rhine");
    EXPECT_EQ(wordle.count(&result1), 24);
    vector<string> expected1{
        "arise", "bribe", "bride", "brise", "brize", "crime", "cripe", "crise",
        "drice", "drive", "frise", "frize", "grice", "gride", "grike", "grime",
        "gripe", "grise", "grize", "price", "pride", "prime", "prise", "prize",
    };
    EXPECT_EQ(result1, expected1);

    vector<string> result2;
    wordle.guess("arise");
    EXPECT_EQ(wordle.count(&result2), 18);
    vector<string> expected2{
        "bribe", "bride", "brize", "crime", "cripe", "drice",
        "drive", "frize", "grice", "gride", "grike", "grime",
        "gripe", "grize", "price", "pride", "prime", "prize",
    };
    EXPECT_EQ(result2, expected2);

    vector<string> result3;
    wordle.guess("bribe");
    EXPECT_EQ(wordle.count(&result3), 1);
    vector<string> expected3{ "bribe" };
    EXPECT_EQ(result3, expected3);

    EXPECT_TRUE(wordle.isGameOver());
    EXPECT_EQ(wordle.getStatus(), Wordle::GameStatus::WON);
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

    query1.include('b');
    EXPECT_EQ(trie.count(query1), 25);

    query1.setMisplaced('b', 0);
    EXPECT_EQ(trie.count(query1), 12);

    query1.exclude('s');
    vector<string> result1;
    EXPECT_EQ(trie.count(query1, &result1), 8);
    vector<string> expected1{ "cabal", "kabab", "kabar", "maban",
                              "nabam", "nawab", "rabat", "tabac" };
    EXPECT_EQ(result1, expected1);

    auto query2 = trie.query();
    query2.setCorrect('e', 1);
    EXPECT_EQ(trie.count(query2), 1857);

    query2.include("at");
    EXPECT_EQ(trie.count(query2), 110);

    query2.setMisplaced("sa", 4);
    EXPECT_EQ(trie.count(query2), 53);

    query2.exclude("id");
    vector<string> result2;
    EXPECT_EQ(trie.count(query2, &result2), 45);
    vector<string> expected2{
        "beast", "beath", "beaty", "beaut", "begat", "bepat", "besat", "feart",
        "feast", "fetal", "heart", "heast", "heath", "heaty", "leant", "leapt",
        "least", "meant", "meath", "meaty", "metal", "neant", "neath", "neato",
        "peart", "peaty", "petal", "petar", "react", "reast", "reate", "repat",
        "resat", "retag", "retam", "retax", "setae", "setal", "teach", "teary",
        "tease", "teaze", "telae", "tepal", "yeast"
    };
    EXPECT_EQ(result2, expected2);
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