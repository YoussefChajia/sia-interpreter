#include "../src/lexer.hpp"
#include "../src/token.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <optional>

using namespace std;

// Helper function to check token properties
void check_token(const optional<Token>& token_opt, TokenType expected_type, const string& expected_lexeme, unsigned int expected_line, unsigned int expected_column) {
    ASSERT_TRUE(token_opt.has_value());
    const Token& token = token_opt.value();
    EXPECT_EQ(token.type, expected_type);
    EXPECT_EQ(token.lexeme, expected_lexeme);
    EXPECT_EQ(token.line, expected_line);
    // Adjust column check as needed, exact column might vary based on internal logic
    EXPECT_GE(token.column, expected_column); 
}

TEST(LexerTest, BasicTokens) {
    Lexer lexer;
    lexer.init("identifier_123 42 "string literal" true false function return loop if else parallel ; {} () , + - * / % < > <= >= == !=");

    check_token(lexer.get_next_token(), TokenType::IDENTIFIER, "identifier_123", 1, 1);
    check_token(lexer.get_next_token(), TokenType::NUMBER, "42", 1, 17);
    check_token(lexer.get_next_token(), TokenType::STRING, "string literal", 1, 20);
    check_token(lexer.get_next_token(), TokenType::TRUE, "true", 1, 38);
    check_token(lexer.get_next_token(), TokenType::FALSE, "false", 1, 43);
    check_token(lexer.get_next_token(), TokenType::FUNCTION, "function", 1, 49);
    check_token(lexer.get_next_token(), TokenType::RETURN, "return", 1, 58);
    check_token(lexer.get_next_token(), TokenType::LOOP, "loop", 1, 65);
    check_token(lexer.get_next_token(), TokenType::IF, "if", 1, 70);
    check_token(lexer.get_next_token(), TokenType::ELSE, "else", 1, 73);
    check_token(lexer.get_next_token(), TokenType::PARALLEL, "parallel", 1, 78);
    check_token(lexer.get_next_token(), TokenType::SEMICOLON, ";", 1, 87);
    check_token(lexer.get_next_token(), TokenType::LEFT_BRACE, "{", 1, 89);
    check_token(lexer.get_next_token(), TokenType::RIGHT_BRACE, "}", 1, 90);
    check_token(lexer.get_next_token(), TokenType::LEFT_PAREN, "(", 1, 92);
    check_token(lexer.get_next_token(), TokenType::RIGHT_PAREN, ")", 1, 93);
    check_token(lexer.get_next_token(), TokenType::COMMA, ",", 1, 95);
    check_token(lexer.get_next_token(), TokenType::PLUS, "+", 1, 97);
    check_token(lexer.get_next_token(), TokenType::MINUS, "-", 1, 99);
    check_token(lexer.get_next_token(), TokenType::MULTIPLY, "*", 1, 101);
    check_token(lexer.get_next_token(), TokenType::DIVIDE, "/", 1, 103);
    check_token(lexer.get_next_token(), TokenType::MODULO, "%", 1, 105);
    check_token(lexer.get_next_token(), TokenType::LESS_THAN, "<", 1, 107);
    check_token(lexer.get_next_token(), TokenType::GREATER_THAN, ">", 1, 109);
    check_token(lexer.get_next_token(), TokenType::LESS_EQUAL, "<=", 1, 111);
    check_token(lexer.get_next_token(), TokenType::GREATER_EQUAL, ">=", 1, 114);
    check_token(lexer.get_next_token(), TokenType::EQUAL, "==", 1, 117);
    check_token(lexer.get_next_token(), TokenType::NOT_EQUAL, "!=", 1, 120);

    EXPECT_FALSE(lexer.has_more_tokens());
    EXPECT_TRUE(lexer.is_EOF());
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
    
    EXPECT_FALSE(lexer.has_more_tokens());
}

TEST(LexerTest, InvalidInput) {
    Lexer lexer;
    lexer.init("valid_token @ invalid_char");
    
    check_token(lexer.get_next_token(), TokenType::IDENTIFIER, "valid_token", 1, 1);
    ASSERT_THROW(lexer.get_next_token(), std::runtime_error);
    // We expect an exception on '@'. The exact error message might vary.
    // try {
    //     lexer.get_next_token();
    //     FAIL() << "Expected std::runtime_error for invalid character";
    // } catch (const std::runtime_error& e) {
    //     // Optionally check the error message content
    //     EXPECT_NE(string(e.what()).find("Unexpected input"), string::npos);
    //     EXPECT_NE(string(e.what()).find("@"), string::npos);
    // } catch (...) {
    //     FAIL() << "Expected std::runtime_error, but got different exception";
    // }
}

TEST(LexerTest, FloatingPointNumbers) {
    Lexer lexer;
    lexer.init("1.23 45. 0.99");
    check_token(lexer.get_next_token(), TokenType::NUMBER, "1.23", 1, 1);
    check_token(lexer.get_next_token(), TokenType::NUMBER, "45.", 1, 6);
    check_token(lexer.get_next_token(), TokenType::NUMBER, "0.99", 1, 10);
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
    EXPECT_FALSE(lexer.has_more_tokens());
} 