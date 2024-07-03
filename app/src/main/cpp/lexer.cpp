#include <string>
#include <vector>
#include <iostream>
#include "../headers/lexer.h"

using namespace std;

Lexer::Lexer(const string& input) : input(input), position(0), read_position(0), current_char('\0') {
    readChar();
}

void Lexer::eatWhitespaces() {
    while (isspace(current_char)) {
        readChar();
    }
}

void Lexer::eatComments() {
    readChar();
    if (current_char == '/') {
        while (current_char != '\n' && current_char != '\0') {
            readChar();
        }
    } else if (current_char == '*') {
        readChar();
        while (!(current_char == '*' && peekChar() == '/')) {
            if (current_char == '\0') return; // Handle unclosed comments gracefully
            readChar();
        }
        readChar(); // Consume '*'
        readChar(); // Consume '/'
    }
}

vector<Token> Lexer::scanTokens() {
    vector<Token> tokens;
    Token token = NextToken();
    while (token.GetType() != TokenType::EOF_TOKEN) {
        if (token.GetType() != TokenType::COMMENT) {
            tokens.push_back(token);
        }
        token = NextToken();
    }
    return tokens;
}

void Lexer::readChar() {
    if (read_position >= static_cast<int>(input.length())) {
        current_char = '\0';
    } else {
        current_char = input[read_position];
    }
    position = read_position;
    read_position += 1;
}

char Lexer::peekChar() {
    if (static_cast<size_t>(read_position) >= input.size()) {
        return '\0';
    } else {
        return input[read_position];
    }
}

Token Lexer::NextToken() {
    Token token = Token(TokenType::EOF_TOKEN, '\0');

    eatWhitespaces();

    switch (current_char)
    {
        case '(':
            token = Token(TokenType::LEFT_PAREN, current_char);
            break;
        case ')':
            token = Token(TokenType::RIGHT_PAREN, current_char);
            break;
        case '{':
            token = Token(TokenType::LEFT_BRACE, current_char);
            break;
        case '}':
            token = Token(TokenType::RIGHT_BRACE, current_char);
            break;
        case ',':
            token = Token(TokenType::COMMA, current_char);
            break;
        case '+':
            token = Token(TokenType::PLUS, current_char);
            break;
        case '-':
            token = Token(TokenType::MINUS, current_char);
            break;
        case '*':
            token = Token(TokenType::STAR, current_char);
            break;
        case '/':
            if (peekChar() == '/' || peekChar() == '*') {
                eatComments();
                token = Token(TokenType::COMMENT, "//");
            } else {
                token = Token(TokenType::SLASH, current_char);
            }
            break;
        case '%':
            token = Token(TokenType::PERCENT, current_char);
            break;
        case '=':
            if (peekChar() == '=') {
                readChar();
                token = Token(TokenType::EQUAL_EQUAL, "==");
            } else {
                token = Token(TokenType::EQUAL, current_char);
            }
            break;
        case '<':
            if (peekChar() == '=') {
                readChar();
                token = Token(TokenType::LESS_EQUAL, "<=");
            } else {
                token = Token(TokenType::LESS, current_char);
            }
            break;
        case '>':
            if (peekChar() == '=') {
                readChar();
                token = Token(TokenType::GREATER_EQUAL, ">=");
            } else {
                token = Token(TokenType::GREATER, current_char);
            }
            break;
        case '!':
            if (peekChar() == '=') {
                readChar();
                token = Token(TokenType::NOT_EQUAL, "!=");
            }
            break;
        default:
            if (isLetter(current_char)) {
                string identifier = readIdentifier();
                TokenType type = token.LookupIdentifier(identifier);
                return Token(type, identifier);
            } else if (isdigit(current_char)) {
                string number = readNumber();
                return Token(TokenType::NUMBER, number);
            } else if (current_char == '"') {
                readChar();
                string literal = readString();
                token = Token(TokenType::STRING, literal);
            } else if (current_char == '\0') {
                token = Token(TokenType::EOF_TOKEN, current_char);
            } else {
                token = Token(TokenType::ILLEGAL, current_char);
            }
            break;
    }

    readChar();
    return token;
}

string Lexer::readIdentifier() {
    int start_position = position;
    while (isLetter(current_char)) {
        readChar();
    }
    return input.substr(start_position, position - start_position);
}

string Lexer::readNumber() {
    int start_position = position;
    while (isdigit(current_char)) {
        readChar();
    }
    return input.substr(start_position, position - start_position);
}

string Lexer::readString() {
    int start_position = position;
    while (current_char != '"' && current_char != '\0') {
        readChar();
    }
    if (current_char == '"') {
        readChar();
    }
    return input.substr(start_position, position - start_position - 1);
}

bool Lexer::isLetter(char c) {
    return isalpha(c) || c == '_';
}
