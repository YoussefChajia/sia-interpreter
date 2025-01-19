#include "parser.hpp"
#include "lexer.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <iostream>


using namespace std;

Parser::Parser(const string& input) : input_(input), lexer_(input) {}

shared_ptr<Program> Parser::parse() {
    look_ahead_ = lexer_.get_next_token();
    return make_shared<Program>(parse_statement_list());
}

vector<shared_ptr<ExpressionStatement>> Parser::parse_statement_list() {
    vector<shared_ptr<ExpressionStatement>> statement_list;

    while (look_ahead_.has_value()) {
        statement_list.push_back(parse_statement());
    }

    return statement_list;
}

shared_ptr<ExpressionStatement> Parser::parse_statement() {
    return parse_expression_statement();
}

shared_ptr<ExpressionStatement> Parser::parse_expression_statement() {
    const shared_ptr<Expression> expression = parse_expression();
    eat(TokenType::SEMICOLON);
    return make_shared<ExpressionStatement>(expression);
}

shared_ptr<Expression> Parser::parse_expression() {
    return parse_literal();
}

shared_ptr<Literal> Parser::parse_literal() {
    if (!look_ahead_.has_value()) {
        throw runtime_error("Unexpected end of input");
    }

    switch (look_ahead_->type) {
        case TokenType::NUMBER: return parse_numeric_literal();
        case TokenType::STRING: return parse_string_literal();
        default: throw runtime_error("Literal: Unexpected literal production");
    }
}

shared_ptr<StringLiteral> Parser::parse_string_literal() {
    Token token = eat(TokenType::STRING);
    // Removing quotes from the string value
    string value = token.value.substr(1, token.value.length() - 2);
    return make_shared<StringLiteral>(value);
}

shared_ptr<NumericLiteral> Parser::parse_numeric_literal() {
    Token token = eat(TokenType::NUMBER);
    return make_shared<NumericLiteral>(stod(token.value));
}

string Parser::token_type_to_string(const TokenType& token_type) {
    map<TokenType, string> type_to_string = {
        {NUMBER, "NUMBER"},
        {STRING, "STRING"},
        {SEMICOLON, "SEMICOLON"}
    };

    return type_to_string[token_type];
}

Token Parser::eat(const TokenType& token_type) {
    if (!look_ahead_.has_value()) {
        throw runtime_error("Unexpected end of input, expected: " + token_type_to_string(token_type));
    }

    if (look_ahead_->type != token_type) {
        throw runtime_error("Unexpected token: " + look_ahead_->value + ", expected: " + token_type_to_string(token_type));
    }

    Token token = look_ahead_.value();
    look_ahead_ = lexer_.get_next_token();
    return token;
}

void Parser::print_indent(int indent) {
    cout << string(indent * 2, ' ');
}

void Parser::print_comma(bool last_item) {
    cout << (last_item ? "" : ",") << endl;
}

void Parser::print_ast(const shared_ptr<Node>& node, int indent) {
    print_indent(indent);
    cout << "{" << endl;
    
    print_indent(indent + 1);
    cout << "\"type\": \"" << node->type << "\"";

    if (auto program = dynamic_pointer_cast<Program>(node)) {
        cout << "," << endl;
        print_indent(indent + 1);
        cout << "\"body\": [" << endl;
        
        for (size_t i = 0; i < program->body.size(); ++i) {
            print_ast(program->body[i], indent + 2);
            print_comma(i == program->body.size() - 1);
        }
        
        print_indent(indent + 1);
        cout << "]";
    }
    else if (auto expr_stmt = dynamic_pointer_cast<ExpressionStatement>(node)) {
        cout << "," << endl;
        print_indent(indent + 1);
        cout << "\"expression\": " << endl;
        print_ast(expr_stmt->expression, indent + 1);
    }
    else if (auto str_literal = dynamic_pointer_cast<StringLiteral>(node)) {
        cout << "," << endl;
        print_indent(indent + 1);
        cout << "\"value\": \"" << str_literal->value << "\"";
    }
    else if (auto num_literal = dynamic_pointer_cast<NumericLiteral>(node)) {
        cout << "," << endl;
        print_indent(indent + 1);
        cout << "\"value\": " << num_literal->value;
    }
    
    cout << endl;
    print_indent(indent);
    cout << "}";
}
