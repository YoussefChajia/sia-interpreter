#pragma once

#include <string>

enum TokenType {
    // identifiers
    IDENTIFIER,
    // spaces
    NEWLINE,
    // literals
    NUMBER, STRING, TRUE, FALSE,
    // keywords
    FUNCTION, RETURN, LOOP,
    // symbols
    LEFT_BRACE, RIGHT_BRACE, LEFT_PAREN, RIGHT_PAREN, ASSIGN, COMMA, SEMICOLON, 
    // operators with order of precedence
    MULTIPLY, DIVIDE, MODULO,
    PLUS, MINUS, 
    LESS_THAN, GREATER_THAN, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR,
};

struct Token {
    TokenType type;
    std::string lexeme;
    unsigned int line, column;

    Token(TokenType type, std::string lexeme, unsigned int line, unsigned int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
};
