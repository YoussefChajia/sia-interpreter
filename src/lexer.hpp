#pragma once

#include <string>
#include <vector>
#include <regex>
#include <optional>

using namespace std;

enum TokenType {
    // Identifiers
    IDENTIFIER,

    // Literals
    NUMBER,
    STRING,

    // Assignments
    SIMPLE_ASSIGN,

    // Symbols
    SEMICOLON,
    LEFT_BRACE,
    RIGHT_BRACE
};

struct Token {
    TokenType type;
    string value;
};

struct Rule {
    regex pattern;
    optional<TokenType> token_type;
};

class Lexer {
public:
    Lexer();
    void init(const string& input);
    bool has_more_tokens() const;
    bool is_EOF() const;
    optional<Token> get_next_token();
    optional<string> match(const regex& pattern, const string& str);

private:
    string input_;
    size_t cursor_;
    vector<pair<regex, optional<TokenType>>> spec_;
};
