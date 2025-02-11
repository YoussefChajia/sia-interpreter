#include <optional>
#include <string>

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
        {regex(R"(^\()"), TokenType::LEFT_PAREN},
        {regex(R"(^\))"), TokenType::RIGHT_PAREN},
        {regex(R"(^,)"), TokenType::COMMA},

        {regex(R"(^<=)"), TokenType::LESS_EQUAL},
        {regex(R"(^>=)"), TokenType::GREATER_EQUAL},
        {regex(R"(^==)"), TokenType::EQUAL},
        {regex(R"(^!=)"), TokenType::NOT_EQUAL},
        {regex(R"(^and\b)"), TokenType::LOGICAL_AND},
        {regex(R"(^or\b)"), TokenType::LOGICAL_OR},

        {regex(R"(^\+)"), TokenType::PLUS},
        {regex(R"(^\-)"), TokenType::MINUS},
        {regex(R"(^\*)"), TokenType::MULTIPLY},
        {regex(R"(^\/)"), TokenType::DIVIDE},
        {regex(R"(^%)"), TokenType::MODULO},

        {regex(R"(^<)"), TokenType::LESS_THAN},
        {regex(R"(^>)"), TokenType::GREATER_THAN},
        {regex(R"(^=)"), TokenType::ASSIGN},

        // keywords
        {regex(R"(^function\b)"), TokenType::FUNCTION}, // '\b' ensures that the keyword is not matched if it's a part of a larger word
        {regex(R"(^return\b)"), TokenType::RETURN},
        {regex(R"(^loop\b)"), TokenType::LOOP},
        {regex(R"(^if\b)"), TokenType::IF},
        {regex(R"(^else\b)"), TokenType::ELSE},

        // literals
        {regex(R"(^\d+\.?\d*)"), TokenType::NUMBER},
        {regex(R"(^"[^"]*")"), TokenType::STRING},
        {regex(R"(^true\b)"), TokenType::TRUE},
        {regex(R"(^false\b)"), TokenType::FALSE},

        // identifiers
        {regex(R"(^[a-zA-Z][a-zA-Z0-9_]*)"), TokenType::IDENTIFIER},
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

// TODO -> optimize looking thru spec_
//
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

    throw runtime_error("Unexpected input: \"" + string(1, input[0]) + "\" at " + to_string(line_) + ", " + to_string(column_));
}

optional<string> Lexer::match(const regex& pattern, const string& str) {
    smatch matched;
    if (!regex_search(str, matched, pattern)) return nullopt;
    cursor_ += matched[0].length();
    return matched[0];
}
