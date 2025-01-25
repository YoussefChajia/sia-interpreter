#pragma once

#include <string>
#include <optional>
#include <regex>

#include "token.hpp"

using namespace std;

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
    unsigned int cursor_;
    unsigned int line_;
    unsigned int column_;
    vector<pair<regex, optional<TokenType>>> spec_;
};
