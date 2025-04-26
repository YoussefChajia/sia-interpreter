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
        {regex(R"(^parallel\b)"), TokenType::PARALLEL},

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

optional<Token> Lexer::get_next_token() {
    if (!has_more_tokens()) return nullopt;

    const string current_input = input_.substr(cursor_);

    for (const auto& [regex, token_type_opt] : spec_) {
        smatch matched;
        if (regex_search(current_input, matched, regex)) {
            if (matched.position(0) != 0) continue;

            string matched_str = matched[0].str();
            unsigned int match_length = matched_str.length();
            unsigned int start_column = column_;

            cursor_ += match_length;

            if (!token_type_opt.has_value()) {
                 size_t last_newline = matched_str.rfind('\n');
                 if (last_newline != string::npos) {
                     unsigned int newline_count = 0;
                     for(char c : matched_str) { if (c == '\n') newline_count++; }
                     line_ += newline_count;
                     column_ = matched_str.length() - last_newline;
                 } else {
                     column_ += match_length;
                 }
                 return get_next_token();
            }

            TokenType token_type = token_type_opt.value();

            if (token_type == TokenType::NEWLINE) {
                line_++;
                column_ = 1;
                return get_next_token();
            }

            string lexeme = matched_str;
            if (token_type == TokenType::STRING) {
                 if (lexeme.length() >= 2 && lexeme.front() == '"' && lexeme.back() == '"') {
                    lexeme = lexeme.substr(1, lexeme.size() - 2);
                 } else {
                    // Maybe throw error for unterminated string literal
                 }
            }

            column_ += match_length; 

            return Token{token_type, lexeme, line_, start_column};
        }
    }

    // If the loop finishes without matching any rule
    throw runtime_error("Unexpected input: \"" + string(1, current_input[0]) + "\" at " + to_string(line_) + ", " + to_string(column_));
}
