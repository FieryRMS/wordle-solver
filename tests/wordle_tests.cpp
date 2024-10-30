#include <gtest/gtest.h>
#include "wordle.h"

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