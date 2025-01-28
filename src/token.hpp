#pragma once

#include <string>

enum TokenType {
    // Identifiers
    IDENTIFIER,
    // Spaces
    NEWLINE,
    // Literals
    NUMBER, STRING, TRUE, FALSE,
    // keywords
    PRINT, FUNCTION, RETURN, LOOP,
    // Symbols
    LEFT_BRACE, RIGHT_BRACE, LEFT_PAREN, RIGHT_PAREN,
    ASSIGN, COMMA, SEMICOLON, 
    PLUS, MINUS, MULTIPLY, DIVIDE,
};

struct Token {
    TokenType type;
    std::string lexeme;
    unsigned int line, column;

    Token(TokenType type, std::string lexeme, unsigned int line, unsigned int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
};
