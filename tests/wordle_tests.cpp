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
         expected = vector<Wordle::TileType>{ Wordle::TileType::WRONG,
                                              Wordle::TileType::WRONG,
                                              Wordle::TileType::WRONG,
                                              Wordle::TileType::WRONG,
                                              Wordle::TileType::MISPLACED };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("kiaat").result, expected = vector<Wordle::TileType>{
        Wordle::TileType::WRONG, Wordle::TileType::WRONG,
        Wordle::TileType::MISPLACED, Wordle::TileType::MISPLACED,
        Wordle::TileType::WRONG
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("mahal").result, expected = vector<Wordle::TileType>{
        Wordle::TileType::WRONG, Wordle::TileType::CORRECT,
        Wordle::TileType::CORRECT, Wordle::TileType::MISPLACED,
        Wordle::TileType::WRONG
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("shahs").result, expected = vector<Wordle::TileType>{
        Wordle::TileType::WRONG, Wordle::TileType::MISPLACED,
        Wordle::TileType::MISPLACED, Wordle::TileType::WRONG,
        Wordle::TileType::WRONG
    };

    EXPECT_EQ(result, expected) << "Expected: " << wordle.guess2emoji(expected)
                                << " Got: " << wordle.guess2emoji(result);

    result = wordle.guess("bbaaa").result, expected = vector<Wordle::TileType>{
        Wordle::TileType::WRONG, Wordle::TileType::WRONG,
        Wordle::TileType::MISPLACED, Wordle::TileType::MISPLACED,
        Wordle::TileType::WRONG
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

TEST(WORDLE, COUNT4)
{
    Wordle wordle("eches");
    auto stat = wordle.getStat(-1);
    EXPECT_EQ(stat.count, 14855);

    stat = wordle.guess("tares");
    EXPECT_EQ(stat.count, 375);

    stat = wordle.guess("pilon");
    EXPECT_EQ(stat.count, 78);

    stat = wordle.guess("demob");
    EXPECT_EQ(stat.count, 22);
    vector<string> result = wordle.getWords(-1);
    vector<string> expected = {
        "cukes", "cuzes", "eches", "eques", "esses", "fuses", "fuzes", "fyces",
        "fykes", "ghees", "gyves", "hykes", "jukes", "juves", "scyes", "skees",
        "suses", "swees", "syces", "sykes", "wuses", "yukes",
    };

    EXPECT_EQ(result, expected);

    stat = wordle.guess("skees");
    EXPECT_EQ(stat.count, 7);
    result = wordle.getWords(-1);
    expected = {
        "cuzes", "eches", "eques", "fuzes", "fyces", "gyves", "juves",
    };
    EXPECT_EQ(result, expected);
}

TEST(WORDLE, QUERY_EXISTS)
{
    Wordle wordle("eches");
    auto stat = wordle.guess("tares");
    EXPECT_TRUE(stat.query.verify("eches"));

    stat = wordle.guess("pilon");
    EXPECT_TRUE(stat.query.verify("eches"));

    stat = wordle.guess("demob");
    EXPECT_TRUE(stat.query.verify("eches"));

    // stat = wordle.guess("skees");
    // EXPECT_TRUE(stat.query.test("eches"));

    stat = wordle.guess("lucky");
    EXPECT_TRUE(stat.query.verify("eches"));

    EXPECT_FALSE(stat.query.verify("esses"));
    EXPECT_FALSE(stat.query.verify("ghees"));
    EXPECT_FALSE(stat.query.verify("swees"));
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
TEST(TRIE, PATTERNCOUNTS1)
{
    string guess = "camus";
    vector<string> words = { "beisa", "fossa", "plush", "queck",
                             "rossa", "sputa", "squad", "camus" };
    Trie<5> trie;
    for (auto &w : words) trie.insert(w);
    auto query = trie.query("");
    auto result = trie.getPatternsCounts(guess, query);
    map<string, int> expected = {
        { "CCCCC", 1 }, { "MWWMW", 1 }, { "WMWMM", 2 },
        { "WMWWM", 3 }, { "WWWMM", 1 },
    };
    EXPECT_EQ(result, expected);
}

TEST(TRIE, PATTERNCOUNTS2)
{
    string guess = "goory";
    vector<string> words = { "snool", "goory" };
    Trie<5> trie;
    for (auto &w : words) trie.insert(w);
    auto query = trie.query("");
    auto result = trie.getPatternsCounts(guess, query);
    map<string, int> expected = {
        { "CCCCC", 1 },
        { "WMCWW", 1 },
    };
    EXPECT_EQ(result, expected);
}

TEST(TRIE, PATTERNCOUNTS3)
{
    ifstream file(filepath);
    ASSERT_TRUE(file.is_open());

    string word;
    Trie<5> trie;
    while (file >> word) trie.insert(word);

    auto query = trie.query("");
    auto result = trie.getPatternsCounts("aband", query);

    map<string, int> expected{
        { "CCCCC", 1 },    { "CCCWW", 13 },  { "CCMCW", 1 },
        { "CCMWC", 1 },    { "CCMWW", 8 },   { "CCWCW", 2 },
        { "CCWMW", 5 },    { "CCWWC", 4 },   { "CCWWM", 2 },
        { "CCWWW", 33 },   { "CMCWW", 1 },   { "CMMMW", 1 },
        { "CMMWW", 6 },    { "CMWWC", 1 },   { "CMWWM", 6 },
        { "CMWWW", 10 },   { "CWCCC", 1 },   { "CWCCW", 9 },
        { "CWCMM", 1 },    { "CWCMW", 4 },   { "CWCWC", 4 },
        { "CWCWM", 6 },    { "CWCWW", 58 },  { "CWMCW", 2 },
        { "CWMMM", 2 },    { "CWMMW", 28 },  { "CWMWC", 6 },
        { "CWMWM", 8 },    { "CWMWW", 105 }, { "CWWCC", 2 },
        { "CWWCM", 1 },    { "CWWCW", 42 },  { "CWWMC", 3 },
        { "CWWMM", 10 },   { "CWWMW", 75 },  { "CWWWC", 39 },
        { "CWWWM", 51 },   { "CWWWW", 316 }, { "MCWCW", 1 },
        { "MCWWW", 5 },    { "MMCCW", 1 },   { "MMCWC", 1 },
        { "MMCWM", 1 },    { "MMCWW", 6 },   { "MMMMM", 1 },
        { "MMMMW", 10 },   { "MMMWM", 3 },   { "MMMWW", 47 },
        { "MMWCW", 10 },   { "MMWMC", 1 },   { "MMWMM", 3 },
        { "MMWMW", 54 },   { "MMWWC", 14 },  { "MMWWM", 22 },
        { "MMWWW", 319 },  { "MWCCM", 1 },   { "MWCCW", 14 },
        { "MWCMW", 4 },    { "MWCWC", 2 },   { "MWCWM", 4 },
        { "MWCWW", 62 },   { "MWMCM", 1 },   { "MWMCW", 13 },
        { "MWMMC", 3 },    { "MWMMM", 6 },   { "MWMMW", 79 },
        { "MWMWC", 3 },    { "MWMWM", 30 },  { "MWMWW", 301 },
        { "MWWCC", 2 },    { "MWWCM", 14 },  { "MWWCW", 109 },
        { "MWWMC", 26 },   { "MWWMM", 67 },  { "MWWMW", 508 },
        { "MWWWC", 161 },  { "MWWWM", 287 }, { "MWWWW", 2005 },
        { "WCCCW", 2 },    { "WCCWM", 1 },   { "WCCWW", 5 },
        { "WCWCW", 3 },    { "WCWWC", 2 },   { "WCWWW", 19 },
        { "WMCCC", 2 },    { "WMCCW", 10 },  { "WMCMW", 5 },
        { "WMCWC", 5 },    { "WMCWM", 9 },   { "WMCWW", 88 },
        { "WMWCC", 5 },    { "WMWCW", 39 },  { "WMWMC", 3 },
        { "WMWMM", 12 },   { "WMWMW", 89 },  { "WMWWC", 50 },
        { "WMWWM", 74 },   { "WMWWW", 704 }, { "WWCCC", 7 },
        { "WWCCM", 7 },    { "WWCCW", 114 }, { "WWCMM", 4 },
        { "WWCMW", 77 },   { "WWCWC", 32 },  { "WWCWM", 101 },
        { "WWCWW", 711 },  { "WWWCC", 31 },  { "WWWCM", 27 },
        { "WWWCW", 411 },  { "WWWMC", 61 },  { "WWWMM", 175 },
        { "WWWMW", 1063 }, { "WWWWC", 401 }, { "WWWWM", 746 },
        { "WWWWW", 4691 }
    };

    ASSERT_EQ(result.size(), expected.size());

    // too big to compare and show the difference, break it down
    const int limit = 20;
    vector<map<string, int>> result_parts;
    vector<map<string, int>> expected_parts;
    auto itr = result.begin(), ite = expected.begin();
    while (itr != result.end())
    {
        map<string, int> r, e;
        for (int i = 0; i < limit && itr != result.end(); i++, itr++, ite++)
            r.insert(*itr), e.insert(*ite);

        result_parts.push_back(r);
        expected_parts.push_back(e);
    }

    for (int i = 0; i < result_parts.size(); i++)
    {
        EXPECT_EQ(result_parts[i], expected_parts[i])
            << "OFFSET: " << i * limit;
    }
}

TEST(TRIE, PATTERNCOUNTS4)
{
    ifstream file(filepath);
    ASSERT_TRUE(file.is_open());

    string word;
    Trie<5> trie;
    while (file >> word) trie.insert(word);

    auto query = trie.query("");
    auto result = trie.getPatternsCounts("annan", query);

    map<string, int> expected{
        { "CCCCC", 1 },   { "CCCCW", 3 },   { "CCCWW", 5 },   { "CCMMW", 1 },
        { "CCMWW", 1 },   { "CCWCW", 9 },   { "CCWMW", 4 },   { "CCWWC", 3 },
        { "CCWWW", 31 },  { "CMWMW", 13 },  { "CMWWW", 46 },  { "CWCCW", 1 },
        { "CWCMW", 3 },   { "CWCWW", 14 },  { "CWWCC", 11 },  { "CWWCW", 94 },
        { "CWWMC", 4 },   { "CWWMW", 122 }, { "CWWWC", 40 },  { "CWWWW", 462 },
        { "MCMMW", 1 },   { "MCMWW", 1 },   { "MCWWC", 1 },   { "MCWWW", 53 },
        { "MMCCW", 1 },   { "MMCMM", 1 },   { "MMCMW", 6 },   { "MMCWM", 2 },
        { "MMCWW", 35 },  { "MMMMW", 2 },   { "MMMWW", 3 },   { "MMWCW", 21 },
        { "MMWMW", 33 },  { "MMWWC", 1 },   { "MMWWW", 356 }, { "MWCCW", 18 },
        { "MWCMW", 25 },  { "MWCWC", 2 },   { "MWCWW", 240 }, { "MWWCC", 25 },
        { "MWWCW", 227 }, { "MWWMW", 233 }, { "MWWWC", 112 }, { "MWWWW", 3074 },
        { "WCCWW", 7 },   { "WCWCC", 3 },   { "WCWCW", 27 },  { "WCWWC", 15 },
        { "WCWWW", 222 }, { "WMCCC", 1 },   { "WMCCW", 1 },   { "WMCWC", 1 },
        { "WMCWM", 4 },   { "WMCWW", 69 },  { "WMMWW", 11 },  { "WMWCW", 33 },
        { "WMWWC", 9 },   { "WMWWW", 677 }, { "WWCCC", 1 },   { "WWCCW", 57 },
        { "WWCWC", 11 },  { "WWCWW", 610 }, { "WWWCC", 94 },  { "WWWCW", 691 },
        { "WWWWC", 283 }, { "WWWWW", 6687 }
    };

    ASSERT_EQ(result.size(), expected.size());

    // too big to compare and show the difference, break it down
    const int limit = 20;
    vector<map<string, int>> result_parts;
    vector<map<string, int>> expected_parts;
    auto itr = result.begin(), ite = expected.begin();
    while (itr != result.end())
    {
        map<string, int> r, e;
        for (int i = 0; i < limit && itr != result.end(); i++, itr++, ite++)
            r.insert(*itr), e.insert(*ite);

        result_parts.push_back(r);
        expected_parts.push_back(e);
    }

    for (int i = 0; i < result_parts.size(); i++)
    {
        EXPECT_EQ(result_parts[i], expected_parts[i])
            << "OFFSET: " << i * limit;
    }
}