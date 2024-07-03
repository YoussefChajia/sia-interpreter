#include <string>
#include "../headers/token.h"
#include <vector>

using namespace std;

vector<pair<string, TokenType>> keywords = {
    {"and", TokenType::AND},
    {"do", TokenType::DO},
    {"else", TokenType::ELSE},
    {"elseif", TokenType::ELSEIF},
    {"end", TokenType::END},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"fun", TokenType::FUNCTION},
    {"if", TokenType::IF},
    {"in", TokenType::IN},
    {"nil", TokenType::NIL},
    {"not", TokenType::NOT},
    {"or", TokenType::OR},
    {"return", TokenType::RETURN},
    {"then", TokenType::THEN},
    {"true", TokenType::TRUE},
    {"while", TokenType::WHILE}
};

Token::Token() : type(TokenType::ILLEGAL), lexeme("") {}
Token::Token(TokenType type, const std::string& lexeme) : type(type), lexeme(lexeme) {}
Token::Token(TokenType type, char lexeme) : type(type), lexeme(1, lexeme) {}

TokenType Token::GetType() const { return type; }
string Token::GetLexeme() const { return lexeme; }

void Token::SetType(TokenType type) { this->type = type;}
void Token::SetLexeme(std::string lexeme) { this->lexeme = lexeme;}

TokenType Token::LookupIdentifier(string identifier) {
    for (size_t i = 0; i < keywords.size(); ++i) {
        if (keywords[i].first == identifier) {
            return keywords[i].second;
        }
    }
    return TokenType::IDENTIFIER;
}