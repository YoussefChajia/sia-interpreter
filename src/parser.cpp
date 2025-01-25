#include <string>
#include <vector>

#include "ast.hpp"
#include "parser.hpp"
#include "token.hpp"


using namespace std;

Parser::Parser() {}

unique_ptr<ProgramNode> Parser::parse(const string& input) {
    input_ = input;
    lexer_.init(input);
    look_ahead_ = lexer_.get_next_token();
    return parse_program();
}


unique_ptr<ProgramNode> Parser::parse_program() {
    auto program = make_unique<ProgramNode>();

    while (look_ahead_.has_value()) {
        program->statements.push_back(parse_statement());
    }

    return program;
}


unique_ptr<StatementNode> Parser::parse_statement() {
    switch (look_ahead_->type) {
        case TokenType::IDENTIFIER : return parse_assignment();
        case TokenType::PRINT : return parse_print();
        default: throw runtime_error("Unexpected token: " + look_ahead_->lexeme);
    }
}

unique_ptr<StatementNode> Parser::parse_assignment() {
    Token identifier = eat(TokenType::IDENTIFIER);
    eat(TokenType::ASSIGN);
    auto expression = parse_expression();
    eat(TokenType::SEMICOLON);
    return make_unique<AssignmentNode>(identifier.lexeme, std::move(expression), identifier.line, identifier.column);
}


unique_ptr<ExpressionNode> Parser::parse_expression() {
    return parse_term();
}

unique_ptr<ExpressionNode> Parser::parse_term() {
    auto left = parse_factor();
    // TODO -> Fix getting the token dynamically cuz now the parser expects plus first
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = look_ahead_.value(); eat(op.type);
        auto right = parse_factor();
        left = make_unique<BinaryOpNode>(op.type, std::move(left), std::move(right), op.line, op.column);
    }
    return left;
}

unique_ptr<ExpressionNode> Parser::parse_factor() {
    auto left = parse_primary();
    // TODO -> Fix getting the token dynamically cuz now the parser expects multiply first
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE)) {
        Token op = look_ahead_.value(); eat(op.type);
        auto right = parse_primary();
        left = make_unique<BinaryOpNode>(op.type, std::move(left), std::move(right), op.line, op.column);
    }
    return left;
}

unique_ptr<ExpressionNode> Parser::parse_primary() {
    switch (look_ahead_->type) {
        case TokenType::LEFT_PAREN : {
            eat(TokenType::LEFT_PAREN);
            auto expression = parse_expression();
            eat(TokenType::RIGHT_PAREN);
            return expression;
        }
        case TokenType::IDENTIFIER : {
            Token identifier = eat(TokenType::IDENTIFIER);
            return make_unique<VariableNode>(identifier.lexeme, identifier.line, identifier.column);
        }
        case TokenType::STRING : {
            Token string = eat(TokenType::STRING);
            return make_unique<StringLiteral>(string.lexeme, string.line, string.column);
        }
        case TokenType::NUMBER : {
            Token number = eat(TokenType::NUMBER);
            double value = stod(number.lexeme);
            return make_unique<NumberLiteral>(value, number.line, number.column);
        }
        case TokenType::MINUS : {
            Token op = eat(TokenType::MINUS);
            auto expression = parse_primary();
            return make_unique<UnaryOpNode>(op.type, std::move(expression), op.line, op.column);
        }
        default: throw runtime_error("Unexpected primary token: " + look_ahead_->lexeme);
    }
}


unique_ptr<StatementNode> Parser::parse_print() {
    Token print = eat(TokenType::PRINT);
    eat(TokenType::LEFT_PAREN);
    auto args = parse_print_args();
    eat(TokenType::RIGHT_PAREN);
    eat(TokenType::SEMICOLON);
    return make_unique<PrintNode>(std::move(args), print.line, print.column);
}

vector<unique_ptr<ExpressionNode>> Parser::parse_print_args() {
    vector<unique_ptr<ExpressionNode>> args;
    if (!match(TokenType::RIGHT_PAREN)) {
        args.push_back(parse_expression());
        while (match(TokenType::COMMA)) {
            eat(TokenType::COMMA);
            args.push_back(parse_expression());
        }
    }
    return args;
}


string Parser::token_type_to_string(const TokenType& token_type) {
    map<TokenType, string> type_to_string = {
        {IDENTIFIER, "IDENTIFIER"},
        {NEWLINE, "NEWLINE"},
        {NUMBER, "NUMBER"},
        {STRING, "STRING"},
        {PRINT, "PRINT"},
        {SEMICOLON, "SEMICOLON"},
        {LEFT_BRACE, "LEFT_BRACE"},
        {RIGHT_BRACE, "RIGHT_BRACE"},
        {LEFT_PAREN, "LEFT_PAREN"},
        {RIGHT_PAREN, "RIGHT_PAREN"},
        {ASSIGN, "ASSIGN"},
        {COMMA, "COMMA"},
        {PLUS, "PLUS"},
        {MINUS, "MINUS"},
        {MULTIPLY, "MULTIPLY"},
        {DIVIDE, "DIVIDE"}
    };

    return type_to_string[token_type];
}

Token Parser::eat(const TokenType& token_type) {
    if (!look_ahead_.has_value()) {
        throw runtime_error("Unexpected end of input, expected: " + token_type_to_string(token_type));
    }

    if (look_ahead_->type != token_type) {
        throw runtime_error("Unexpected token: " + look_ahead_->lexeme + ", expected: " + token_type_to_string(token_type));
    }

    Token token = look_ahead_.value();
    look_ahead_ = lexer_.get_next_token();
    return token;
}

bool Parser::match(const TokenType& token_type) {
    return look_ahead_->type == token_type;
}

