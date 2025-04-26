#include "../src/lexer.hpp"
#include "../src/token.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <optional>
#include <iostream>
#include <map>

using namespace std;

// Helper function to check token properties
void check_token(const optional<Token>& token_opt, TokenType expected_type, const string& expected_lexeme, unsigned int expected_line, unsigned int expected_column) {
    ASSERT_TRUE(token_opt.has_value());
    const Token& token = token_opt.value();
    EXPECT_EQ(token.type, expected_type);
    EXPECT_EQ(token.lexeme, expected_lexeme);
    EXPECT_EQ(token.line, expected_line);
    EXPECT_EQ(token.column, expected_column);
}

TEST(LexerTest, BasicTokens) {
    Lexer lexer;
    lexer.init("identifier_123 42 \"string literal\" true false function return loop if else parallel ; {} () , + - * / % < >");

    check_token(lexer.get_next_token(), TokenType::IDENTIFIER, "identifier_123", 1, 1);
    check_token(lexer.get_next_token(), TokenType::NUMBER, "42", 1, 16);
    check_token(lexer.get_next_token(), TokenType::STRING, "string literal", 1, 19);
    check_token(lexer.get_next_token(), TokenType::TRUE, "true", 1, 36);
    check_token(lexer.get_next_token(), TokenType::FALSE, "false", 1, 41);
    check_token(lexer.get_next_token(), TokenType::FUNCTION, "function", 1, 47);
    check_token(lexer.get_next_token(), TokenType::RETURN, "return", 1, 56);
    check_token(lexer.get_next_token(), TokenType::LOOP, "loop", 1, 63);
    check_token(lexer.get_next_token(), TokenType::IF, "if", 1, 68);
    check_token(lexer.get_next_token(), TokenType::ELSE, "else", 1, 71);
    check_token(lexer.get_next_token(), TokenType::PARALLEL, "parallel", 1, 76);
    check_token(lexer.get_next_token(), TokenType::SEMICOLON, ";", 1, 85);
    check_token(lexer.get_next_token(), TokenType::LEFT_BRACE, "{", 1, 87);
    check_token(lexer.get_next_token(), TokenType::RIGHT_BRACE, "}", 1, 88);
    check_token(lexer.get_next_token(), TokenType::LEFT_PAREN, "(", 1, 90);
    check_token(lexer.get_next_token(), TokenType::RIGHT_PAREN, ")", 1, 91);
    check_token(lexer.get_next_token(), TokenType::COMMA, ",", 1, 93);
    check_token(lexer.get_next_token(), TokenType::PLUS, "+", 1, 95);
    check_token(lexer.get_next_token(), TokenType::MINUS, "-", 1, 97);
    check_token(lexer.get_next_token(), TokenType::MULTIPLY, "*", 1, 99);
    check_token(lexer.get_next_token(), TokenType::DIVIDE, "/", 1, 101);
    check_token(lexer.get_next_token(), TokenType::MODULO, "%", 1, 103);
    check_token(lexer.get_next_token(), TokenType::LESS_THAN, "<", 1, 105);
    check_token(lexer.get_next_token(), TokenType::GREATER_THAN, ">", 1, 107);

    EXPECT_EQ(lexer.get_next_token(), std::nullopt);
    EXPECT_TRUE(lexer.is_EOF());
    EXPECT_FALSE(lexer.has_more_tokens());
}

TEST(LexerTest, CommentsAndWhitespace) {
    Lexer lexer;
    lexer.init(R"(
        // This is a single line comment
        variable1 = 10; /* This is a
                           multi-line comment */
        variable2 = 20; // Another comment
    )");

    check_token(lexer.get_next_token(), TokenType::IDENTIFIER, "variable1", 3, 9);
    check_token(lexer.get_next_token(), TokenType::ASSIGN, "=", 3, 19);
    check_token(lexer.get_next_token(), TokenType::NUMBER, "10", 3, 21);
    check_token(lexer.get_next_token(), TokenType::SEMICOLON, ";", 3, 23);
    check_token(lexer.get_next_token(), TokenType::IDENTIFIER, "variable2", 5, 9);
    check_token(lexer.get_next_token(), TokenType::ASSIGN, "=", 5, 19);
    check_token(lexer.get_next_token(), TokenType::NUMBER, "20", 5, 21);
    check_token(lexer.get_next_token(), TokenType::SEMICOLON, ";", 5, 23);
    
    EXPECT_EQ(lexer.get_next_token(), std::nullopt);
    EXPECT_TRUE(lexer.is_EOF());
    EXPECT_FALSE(lexer.has_more_tokens());
}

TEST(LexerTest, InvalidInput) {
    Lexer lexer;
    lexer.init("valid_token @ invalid_char");
    
    check_token(lexer.get_next_token(), TokenType::IDENTIFIER, "valid_token", 1, 1);
    ASSERT_THROW(lexer.get_next_token(), std::runtime_error);
}

TEST(LexerTest, FloatingPointNumbers) {
    Lexer lexer;
    lexer.init("1.23 45. 0.99");
    check_token(lexer.get_next_token(), TokenType::NUMBER, "1.23", 1, 1);
    check_token(lexer.get_next_token(), TokenType::NUMBER, "45.", 1, 6);
    check_token(lexer.get_next_token(), TokenType::NUMBER, "0.99", 1, 10);
    EXPECT_EQ(lexer.get_next_token(), std::nullopt);
    EXPECT_TRUE(lexer.is_EOF());
    EXPECT_FALSE(lexer.has_more_tokens());
}

TEST(LexerTest, LogicalOperators) {
    Lexer lexer;
    lexer.init("true and false or true");
    check_token(lexer.get_next_token(), TokenType::TRUE, "true", 1, 1);
    check_token(lexer.get_next_token(), TokenType::LOGICAL_AND, "and", 1, 6);
    check_token(lexer.get_next_token(), TokenType::FALSE, "false", 1, 10);
    check_token(lexer.get_next_token(), TokenType::LOGICAL_OR, "or", 1, 16);
    check_token(lexer.get_next_token(), TokenType::TRUE, "true", 1, 19);
    EXPECT_EQ(lexer.get_next_token(), std::nullopt);
    EXPECT_TRUE(lexer.is_EOF());
    EXPECT_FALSE(lexer.has_more_tokens());
} 
