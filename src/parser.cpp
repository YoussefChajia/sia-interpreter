#include "parser.hpp"
#include "lexer.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>


using namespace std;

Parser::Parser() {}

shared_ptr<Program> Parser::parse(const string& input) {
    input_ = input;
    lexer_.init(input);
    look_ahead_ = lexer_.get_next_token();
    return make_shared<Program>(parse_statement_list());
}

// ==================================================

vector<shared_ptr<Statement>> Parser::parse_statement_list() {
    vector<shared_ptr<Statement>> statement_list;

    while (look_ahead_.has_value()) {
        statement_list.push_back(parse_statement());
    }

    return statement_list;
}

shared_ptr<Statement> Parser::parse_statement() {
    switch (look_ahead_->type) {
        case TokenType::LEFT_BRACE: return parse_block_statement();
        default: return parse_expression_statement();
    }
}

// ==================================================

shared_ptr<BlockStatement> Parser::parse_block_statement() {
    eat(TokenType::LEFT_BRACE);
    vector<shared_ptr<Statement>> block;

    while (look_ahead_->type != TokenType::RIGHT_BRACE) {
        block.push_back(parse_statement());
    }

    eat(TokenType::RIGHT_BRACE);
    return make_shared<BlockStatement>(block);
}

shared_ptr<ExpressionStatement> Parser::parse_expression_statement() {
    const shared_ptr<Expression> expression = parse_expression();
    eat(TokenType::SEMICOLON);
    return make_shared<ExpressionStatement>(expression);
}

shared_ptr<Expression> Parser::parse_expression() {
    if (!look_ahead_.has_value()) {
        throw runtime_error("Unexpected end of input");
    }

    switch (look_ahead_->type) {
        case TokenType::IDENTIFIER: return parse_identifier();
        case TokenType::NUMBER: return parse_numeric_literal();
        case TokenType::STRING: return parse_string_literal();
        default: throw runtime_error("Literal: Unexpected literal production");
    }
}

// ==================================================

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

shared_ptr<Expression> Parser::parse_identifier() {
    string name = look_ahead_->value;
    eat(TokenType::IDENTIFIER);
    
    if (look_ahead_.has_value() && look_ahead_->type == TokenType::SIMPLE_ASSIGN) {
        eat(TokenType::SIMPLE_ASSIGN);
        shared_ptr<Expression> expression = parse_expression();
        return make_shared<AssignmentExpression>(name, expression);
    }
    
    return make_shared<Identifier>(name);
}

// ==================================================

string Parser::token_type_to_string(const TokenType& token_type) {
    map<TokenType, string> type_to_string = {
        {IDENTIFIER, "IDENTIFIER"},
        {NUMBER, "NUMBER"},
        {STRING, "STRING"},
        {SIMPLE_ASSIGN, "SIMPLE_ASSIGN"},
        {SEMICOLON, "SEMICOLON"},
        {LEFT_BRACE, "LEFT_BRACE"},
        {RIGHT_BRACE, "RIGHT_BRACE"}
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
