#include <gtest/gtest.h>
#include "../src/lexer/Lexer.h"

// Helper: tokenize and return the list (excluding END_OF_INPUT)
static std::vector<Token> lex(const std::string& input) {
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    // Drop the trailing END_OF_INPUT
    if (!tokens.empty() && tokens.back().type == TokenType::END_OF_INPUT) {
        tokens.pop_back();
    }
    return tokens;
}

TEST(LexerTest, EmptyInput) {
    auto tokens = lex("");
    EXPECT_TRUE(tokens.empty());
}

TEST(LexerTest, SingleKeyword) {
    auto tokens = lex("exit");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].value, "exit");
}

TEST(LexerTest, KeywordAndIdentifier) {
    auto tokens = lex("go Documents");
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].value, "go");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].value, "Documents");
}

TEST(LexerTest, PrepositionRecognized) {
    auto tokens = lex("go to Documents");
    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[1].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[1].value, "to");
}

TEST(LexerTest, QuotedString) {
    auto tokens = lex("copy \"my file.txt\" to Backup");
    ASSERT_GE(tokens.size(), 1u);
    EXPECT_EQ(tokens[1].type, TokenType::STRING);
    EXPECT_EQ(tokens[1].value, "my file.txt");
}

TEST(LexerTest, DotDot) {
    auto tokens = lex("go ..");
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[1].type, TokenType::DOTDOT);
}

TEST(LexerTest, CaseInsensitiveKeywords) {
    auto tokens = lex("COPY");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].value, "copy"); // normalized to lowercase
}
