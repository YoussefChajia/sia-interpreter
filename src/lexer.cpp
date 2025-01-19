#include "lexer.hpp"
#include <optional>

using namespace std;

Lexer::Lexer(const string& input) : input_(input), cursor_(0) {
    spec_ = {
        // Whitespaces
        {regex(R"(^\s+)"), nullopt},

        // Comments
        {regex(R"(^\/\/.*)"), nullopt},
        {regex(R"(^\/\*[\s\S]*?\*\/)"), nullopt},

        // Symbols
        {regex(R"(^;)"), TokenType::SEMICOLON},

        // Numbers
        {regex(R"(^\d+)"), TokenType::NUMBER},

        // Strings
        {regex(R"(^"[^"]*")"), TokenType::STRING}
    };
}

bool Lexer::has_more_tokens() const {
    return cursor_ < input_.length();
}

bool Lexer::is_EOF() const {
    return cursor_ == input_.length();
}

optional<Token> Lexer::get_next_token() {
    if (!this->has_more_tokens()) {
        return nullopt;
    }

    const string input = input_.substr(cursor_);

    for (const auto& [regex, token_type] : spec_) {
        const optional<string> token_value = match(regex, input);

        if (token_value == nullopt) {
            continue;
        }

        if (token_type == nullopt) {
            return get_next_token();
        }

        return Token{token_type.value(), token_value.value()};
    }

    throw std::runtime_error("Unexpected token: " + std::string(1, input[0]));
}

optional<string> Lexer::match(const regex& pattern, const string& str) {
    smatch matched;
    if (!regex_search(str, matched, pattern)) {
        return nullopt;
    }
    cursor_ += matched[0].length();
    return matched[0];
}

