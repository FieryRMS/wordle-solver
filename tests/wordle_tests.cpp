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
    auto result = wordle.guess("bruja").result,
         expected = vector<Wordle::TileType>{ Wordle::TileType::NONE,
                                              Wordle::TileType::NONE,
                                              Wordle::TileType::NONE,
                                              Wordle::TileType::NONE,
                                              Wordle::TileType::MISPLACED };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("kiaat").result, expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::NONE,
        Wordle::TileType::MISPLACED, Wordle::TileType::MISPLACED,
        Wordle::TileType::NONE
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("mahal").result, expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::CORRECT,
        Wordle::TileType::CORRECT, Wordle::TileType::MISPLACED,
        Wordle::TileType::NONE
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("shahs").result, expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::MISPLACED,
        Wordle::TileType::MISPLACED, Wordle::TileType::NONE,
        Wordle::TileType::NONE
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("bbaaa").result, expected = vector<Wordle::TileType>{
        Wordle::TileType::NONE, Wordle::TileType::NONE,
        Wordle::TileType::MISPLACED, Wordle::TileType::MISPLACED,
        Wordle::TileType::NONE
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("aahed").result, expected = vector<Wordle::TileType>{
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
    auto stat = wordle.getStat(-1);
    EXPECT_EQ(stat.count, 14855);

    stat = wordle.guess("slate");
    EXPECT_EQ(stat.count, 51);

    stat = wordle.guess("limit");
    EXPECT_EQ(stat.count, 11);
    vector<string> result = wordle.getWords(-1);
    vector<string> expected = {
        "butyl", "hotly", "octyl", "othyl", "thowl", "thurl",
        "tolly", "tolyl", "troll", "trull", "truly",
    };
    EXPECT_EQ(result, expected);

    stat = wordle.guess("truly");
    EXPECT_EQ(stat.count, 1);
    result = wordle.getWords(-1);
    expected = { "thowl" };
    EXPECT_EQ(result, expected);

    stat = wordle.guess("thowl");
    EXPECT_EQ(stat.count, 1);
    result = wordle.getWords(-1);
    expected = { "thowl" };
    EXPECT_EQ(result, expected);

    EXPECT_TRUE(wordle.isGameOver());
    EXPECT_EQ(wordle.getStatus(), Wordle::GameStatus::WON);
}

TEST(WORDLE, COUNT2)
{
    Wordle wordle("breys");
    auto stat = wordle.getStat(-1);
    EXPECT_EQ(stat.count, 14855);

    stat = wordle.guess("warns");
    EXPECT_EQ(stat.count, 308);

    vector<string> result;
    stat = wordle.guess("srsly");
    EXPECT_EQ(stat.count, 7);
    result = wordle.getWords(-1);
    vector<string> expected = {
        "breys", "dreys", "greys", "preys", "treys", "troys", "tryps",
    };
    EXPECT_EQ(result, expected);

    stat = wordle.guess("donas");
    EXPECT_EQ(stat.count, 5);
    result = wordle.getWords(-1);
    expected = {
        "breys", "greys", "preys", "treys", "tryps",
    };
    EXPECT_EQ(result, expected);

    stat = wordle.guess("mynah");
    EXPECT_EQ(stat.count, 5);
    result = wordle.getWords(-1);
    expected = {
        "breys", "greys", "preys", "treys", "tryps",
    };
    EXPECT_EQ(result, expected);

    stat = wordle.guess("treys");
    EXPECT_EQ(stat.count, 3);
    result = wordle.getWords(-1);
    expected = {
        "breys",
        "greys",
        "preys",
    };
    EXPECT_EQ(result, expected);

    stat = wordle.guess("breys");
    EXPECT_EQ(stat.count, 1);
    result = wordle.getWords(-1);
    expected = { "breys" };
    EXPECT_EQ(result, expected);

    EXPECT_TRUE(wordle.isGameOver());
    EXPECT_EQ(wordle.getStatus(), Wordle::GameStatus::WON);
}

TEST(WORDLE, COUNT3)
{
    Wordle wordle("bribe");
    auto stat = wordle.getStat(-1);
    EXPECT_EQ(stat.count, 14855);

    stat = wordle.guess("query");
    EXPECT_EQ(stat.count, 1223);

    stat = wordle.guess("twier");
    EXPECT_EQ(stat.count, 45);

    stat = wordle.guess("rhine");
    EXPECT_EQ(stat.count, 24);
    vector<string> result = wordle.getWords(-1);
    vector<string> expected = {
        "arise", "bribe", "bride", "brise", "brize", "crime", "cripe", "crise",
        "drice", "drive", "frise", "frize", "grice", "gride", "grike", "grime",
        "gripe", "grise", "grize", "price", "pride", "prime", "prise", "prize",
    };
    EXPECT_EQ(result, expected);

    stat = wordle.guess("arise");
    EXPECT_EQ(stat.count, 18);
    result = wordle.getWords(-1);
    expected = {
        "bribe", "bride", "brize", "crime", "cripe", "drice",
        "drive", "frize", "grice", "gride", "grike", "grime",
        "gripe", "grize", "price", "pride", "prime", "prize",
    };
    EXPECT_EQ(result, expected);

    stat = wordle.guess("bribe");
    EXPECT_EQ(stat.count, 1);
    result = wordle.getWords(-1);
    expected = { "bribe" };
    EXPECT_EQ(result, expected);

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
    vector<string> result;
    EXPECT_EQ(trie.count(query1, &result), 8);
    vector<string> expected{ "cabal", "kabab", "kabar", "maban",
                             "nabam", "nawab", "rabat", "tabac" };
    EXPECT_EQ(result, expected);

    auto query2 = trie.query();
    query2.setCorrect('e', 1);
    EXPECT_EQ(trie.count(query2), 1857);

    query2.include("at");
    EXPECT_EQ(trie.count(query2), 110);

    query2.setMisplaced("sa", 4);
    EXPECT_EQ(trie.count(query2), 53);

    query2.exclude("id");
    result.clear();
    EXPECT_EQ(trie.count(query2, &result), 45);
    expected = { "beast", "beath", "beaty", "beaut", "begat", "bepat", "besat",
                 "feart", "feast", "fetal", "heart", "heast", "heath", "heaty",
                 "leant", "leapt", "least", "meant", "meath", "meaty", "metal",
                 "neant", "neath", "neato", "peart", "peaty", "petal", "petar",
                 "react", "reast", "reate", "repat", "resat", "retag", "retam",
                 "retax", "setae", "setal", "teach", "teary", "tease", "teaze",
                 "telae", "tepal", "yeast" };
    EXPECT_EQ(result, expected);
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