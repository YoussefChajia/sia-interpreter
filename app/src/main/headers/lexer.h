#ifndef Lexer_H
#define Lexer_H

#include <string>
#include "token.h"
#include <vector>

using namespace std;

class Lexer {

private:
    string input;
    int position;
    int read_position;
    char current_char;

public:
    Lexer(const string& filename);

    vector<Token> scanTokens();
    void readChar();
    char peekChar();
    Token NextToken();
    string readIdentifier();
    string readNumber();
    string readString();
    bool isLetter(char c);
    void eatWhitespaces();
    void eatComments();
};

#endif // Lexer_H
