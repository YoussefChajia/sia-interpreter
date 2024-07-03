#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

enum TokenType {

    // Punchuation
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, COMMA,
    MINUS, PLUS, SLASH, STAR, NOT_EQUAL, EQUAL, EQUAL_EQUAL,
    LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, PERCENT,
    // Literals
    IDENTIFIER, STRING, NUMBER,
    // Keywords
    AND, OR, DO, ELSE, ELSEIF, END, FALSE, FOR, IF, NIL, NOT,
    FUNCTION, IN, RETURN, THEN, TRUE, WHILE,
    // Comments and EOF
    COMMENT, EOF_TOKEN, ILLEGAL
};

class Token {

private:
    TokenType type;
    string lexeme;

public:
    Token();
    Token(TokenType type, const string& lexeme);
    Token(TokenType type, char lexeme);
    TokenType LookupIdentifier(string identifier);

    TokenType GetType() const;
    string GetLexeme() const;
    void SetType(TokenType type);
    void SetLexeme(string lexeme);
};

#endif // TOKEN_H