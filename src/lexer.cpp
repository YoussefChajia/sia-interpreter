#include <optional>

#include "lexer.hpp"
#include "token.hpp"

using namespace std;

Lexer::Lexer() {
    spec_ = {
        // whitespaces
        {regex(R"(^[ \t\v\f]+)"), nullopt},
        {regex(R"(^(\r\n|\n|\r))"), TokenType::NEWLINE},

        // comments
        {regex(R"(^\/\/.*)"), nullopt},
        {regex(R"(^\/\*[\s\S]*?\*\/)"), nullopt},

        // symbols
        {regex(R"(^;)"), TokenType::SEMICOLON},
        {regex(R"(^\{)"), TokenType::LEFT_BRACE},
        {regex(R"(^\})"), TokenType::RIGHT_BRACE},

        {regex(R"(^\+)"), TokenType::PLUS},
        {regex(R"(^\-)"), TokenType::MINUS},
        {regex(R"(^\*)"), TokenType::MULTIPLY},
        {regex(R"(^\/)"), TokenType::DIVIDE},

        {regex(R"(^\()"), TokenType::LEFT_PAREN},
        {regex(R"(^\))"), TokenType::RIGHT_PAREN},
        {regex(R"(^,)"), TokenType::COMMA},

        // keywords
        // '\b' ensures that the keyword is not matched if it's a part of a larger word
        {regex(R"(^function\b)"), TokenType::FUNCTION},
        {regex(R"(^print\b)"), TokenType::PRINT},
        {regex(R"(^return\b)"), TokenType::RETURN},

        // identifiers
        {regex(R"(^[a-zA-Z][a-zA-Z0-9_]*)"), TokenType::IDENTIFIER},

        // assignments
        {regex(R"(^=)"), TokenType::ASSIGN},

        // literals
        {regex(R"(^\d+\.?\d*)"), TokenType::NUMBER},
        {regex(R"(^"[^"]*")"), TokenType::STRING}
    };
}

void Lexer::init(const string& input) {
    input_ = input;
    cursor_ = 0;
    this->line_ = 1;
    this->column_ = 1;
}

bool Lexer::has_more_tokens() const {
    return cursor_ < input_.length();
}

bool Lexer::is_EOF() const {
    return cursor_ == input_.length();
}

optional<Token> Lexer::get_next_token() {
    if (!this->has_more_tokens()) return nullopt;
    const string input = input_.substr(cursor_);

    for (const auto& [regex, token_type] : spec_) {
        const optional<string> token_value = match(regex, input);

        if (token_value == nullopt) continue;

        if (token_type == nullopt) return get_next_token();

        if (token_type.value() == TokenType::NEWLINE) {
            line_++; column_ = 1;
            return get_next_token();
        }

        string lexeme = token_value.value();
        if (token_type.value() == TokenType::STRING) {
            lexeme = lexeme.substr(1, lexeme.size() - 2);
        }

        column_ += token_value.value().length();
        return Token{token_type.value(), lexeme, this->line_, this->column_};
    }

    throw std::runtime_error("Unexpected token: " + std::string(1, input[0]));
}

optional<string> Lexer::match(const regex& pattern, const string& str) {
    smatch matched;
    if (!regex_search(str, matched, pattern)) return nullopt;
    cursor_ += matched[0].length();
    return matched[0];
}
