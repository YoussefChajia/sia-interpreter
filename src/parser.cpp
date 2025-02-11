#include <memory>
#include <string>
#include <utility>
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
        case TokenType::FUNCTION : return parse_function_def();
        case TokenType::LOOP : return parse_loop();
        case TokenType::IF : return parse_if_else();
        case TokenType::RETURN : return parse_return();
        case TokenType::IDENTIFIER : return parse_identifier();
        case TokenType::LEFT_BRACE : return parse_block();
        default: throw runtime_error("Unexpected token: " + token_type_to_string(look_ahead_->type) + " at (" + to_string(look_ahead_->line) + ", " + to_string(look_ahead_->column) + ")");
    }
}

unique_ptr<ReturnNode> Parser::parse_return() {
    Token token = eat(TokenType::RETURN);
    if (!match(TokenType::SEMICOLON)) {
        auto expression = parse_expression();
        eat(TokenType::SEMICOLON);
        return make_unique<ReturnNode>(std::move(expression), token.line, token.column);
    }
    eat(TokenType::SEMICOLON);
    return make_unique<ReturnNode>(std::move(nullptr), token.line, token.column);
}

unique_ptr<LoopNode> Parser::parse_loop() {
    unique_ptr<ExpressionNode> condition;
    Token loop = eat(TokenType::LOOP);
    eat(TokenType::LEFT_PAREN);
    // TODO : handle loop without an argument
    if (!match(TokenType::RIGHT_PAREN)) {
        condition = parse_expression();
    }
    eat(TokenType::RIGHT_PAREN);
    auto body = parse_block();
    return make_unique<LoopNode>(std::move(condition), std::move(body), loop.line, loop.column);
}

unique_ptr<StatementNode> Parser::parse_identifier() {
    Token identifier = eat(TokenType::IDENTIFIER);
    switch (look_ahead_->type) {
        case TokenType::LEFT_PAREN : {
            auto function_call = parse_function_call(identifier);
            if (match(TokenType::SEMICOLON)) eat(TokenType::SEMICOLON);
            return make_unique<ExpressionStatementNode>(std::move(function_call), identifier.line, identifier.column);
        }
        case TokenType::ASSIGN : return parse_assignment(identifier);
        default: throw runtime_error("Unexpected token: " + token_type_to_string(look_ahead_->type) + " at (" + to_string(look_ahead_->line) + ", " + to_string(look_ahead_->column) + ")");
    }
}

unique_ptr<StatementNode> Parser::parse_assignment(Token& identifier) {
    eat(TokenType::ASSIGN);
    auto expression = parse_expression();
    eat(TokenType::SEMICOLON);
    return make_unique<AssignmentNode>(identifier.lexeme, std::move(expression), identifier.line, identifier.column);
}


unique_ptr<ExpressionNode> Parser::parse_expression() {
    // starting from lowest precedence
    return parse_logical_or();
}

unique_ptr<ExpressionNode> Parser::parse_logical_or() {
    auto left = parse_logical_and();
    while (match(TokenType::LOGICAL_OR)) {
        Token op = look_ahead_.value(); eat(op.type);
        auto right = parse_logical_and();
        left = make_unique<BinaryOpNode>(op.type, std::move(left), std::move(right), op.line, op.column);
    }
    return left;
}

unique_ptr<ExpressionNode> Parser::parse_logical_and() {
    auto left = parse_comparison();
    while (match(TokenType::LOGICAL_AND)) {
        Token op = look_ahead_.value(); eat(op.type);
        auto right = parse_comparison();
        left = make_unique<BinaryOpNode>(op.type, std::move(left), std::move(right), op.line, op.column);
    }
    return left;
}

unique_ptr<ExpressionNode> Parser::parse_comparison() {
    auto left = parse_term();
    while ( match(TokenType::LESS_THAN) || match(TokenType::GREATER_THAN) ||
            match(TokenType::LESS_EQUAL) || match(TokenType::GREATER_EQUAL) ||
            match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL) ) {
        Token op = look_ahead_.value(); eat(op.type);
        auto right = parse_term();
        left = make_unique<BinaryOpNode>(op.type, std::move(left), std::move(right), op.line, op.column);
    }
    return left;
}

unique_ptr<ExpressionNode> Parser::parse_term() {
    auto left = parse_factor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = look_ahead_.value(); eat(op.type);
        auto right = parse_factor();
        left = make_unique<BinaryOpNode>(op.type, std::move(left), std::move(right), op.line, op.column);
    }
    return left;
}

unique_ptr<ExpressionNode> Parser::parse_factor() {
    auto left = parse_primary();
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
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
        case TokenType::TRUE : {
            Token token = eat(TokenType::TRUE);
            return make_unique<BoolLiteral>(true, token.line, token.column);
        }
        case TokenType::FALSE : {
            Token token = eat(TokenType::FALSE);
            return make_unique<BoolLiteral>(false, token.line, token.column);
        }
        case TokenType::STRING : {
            Token string = eat(TokenType::STRING);
            return make_unique<StringLiteral>(string.lexeme, string.line, string.column);
        }
        case TokenType::NUMBER : {
            Token number = eat(TokenType::NUMBER);
            if (is_integer(number.lexeme)) return make_unique<LongNumberLiteral>(stol(number.lexeme), number.line, number.column);
            return make_unique<DoubleNumberLiteral>(stod(number.lexeme), number.line, number.column);
        }
        case TokenType::MINUS : {
            Token op = eat(TokenType::MINUS);
            auto expression = parse_primary();
            return make_unique<UnaryOpNode>(op.type, std::move(expression), op.line, op.column);
        }
        case TokenType::IDENTIFIER : {
            Token identifier = eat(TokenType::IDENTIFIER);
            if (match(TokenType::LEFT_PAREN)) return parse_function_call(identifier);
            return make_unique<VariableNode>(identifier.lexeme, identifier.line, identifier.column);
        }
        default: throw runtime_error("Unexpected primary token: " + token_type_to_string(look_ahead_->type) + " at " + to_string(look_ahead_->line) + ", " + to_string(look_ahead_->column));
    }
}


unique_ptr<FunctionDefNode> Parser::parse_function_def() {
    eat(TokenType::FUNCTION);
    Token name = eat(TokenType::IDENTIFIER);
    vector<string> parameters;
    eat(TokenType::LEFT_PAREN);
    if (!match(TokenType::RIGHT_PAREN)) {
        Token parameter = eat(TokenType::IDENTIFIER);
        parameters.push_back(parameter.lexeme);
        while (match(TokenType::COMMA)) {
            eat(TokenType::COMMA);
            Token parameter = eat(TokenType::IDENTIFIER);
            parameters.push_back(parameter.lexeme);
        }
    }
    eat(TokenType::RIGHT_PAREN);
    auto body = parse_block();
    return make_unique<FunctionDefNode>(std::move(name.lexeme), std::move(parameters), std::move(body), name.line, name.column);
}

unique_ptr<FunctionCallNode> Parser::parse_function_call(Token& name) {
    vector<unique_ptr<ExpressionNode>> arguments;
    eat(TokenType::LEFT_PAREN);
    if (!match(TokenType::RIGHT_PAREN)) {
        arguments.push_back(parse_expression());
        while (match(TokenType::COMMA)) {
            eat(TokenType::COMMA);
            arguments.push_back(parse_expression());
        }
    }
    eat(TokenType::RIGHT_PAREN);
    // eat(TokenType::SEMICOLON);
    return make_unique<FunctionCallNode>(std::move(name.lexeme), std::move(arguments), name.line, name.column);
}

unique_ptr<BlockNode> Parser::parse_block() {
    vector<unique_ptr<StatementNode>> statements;
    Token brace = eat(TokenType::LEFT_BRACE);
    while (!match(TokenType::RIGHT_BRACE)) {
        statements.push_back(parse_statement());
    }
    eat(TokenType::RIGHT_BRACE);
    return make_unique<BlockNode>(std::move(statements), brace.line, brace.column);
}

unique_ptr<IfElseNode> Parser::parse_if_else() {
    unique_ptr<ExpressionNode> condition;
    Token if_token = eat(TokenType::IF);
    eat(TokenType::LEFT_PAREN);
    // TODO : handle if statement without an argument
    if (!match(TokenType::RIGHT_PAREN)) {
        condition = parse_expression();
    }
    eat(TokenType::RIGHT_PAREN);
    auto if_branch = parse_block();
    unique_ptr<BlockNode> else_branch = nullptr;
    if (match(TokenType::ELSE)) {
        eat(TokenType::ELSE);
        if (match(TokenType::IF)) {
            auto nested_if = parse_if_else();
            else_branch = make_unique<BlockNode>(vector<unique_ptr<StatementNode>>(), nested_if->line, nested_if->column);
            else_branch->statements.push_back(std::move(nested_if));
        } else {
            else_branch = parse_block();
        }
    }
    return make_unique<IfElseNode>(std::move(condition), std::move(if_branch), std::move(else_branch), if_token.line, if_token.column);
}

string Parser::token_type_to_string(const TokenType& token_type) {
    map<TokenType, string> type_to_string = {
        {IDENTIFIER, "IDENTIFIER"},
        {NEWLINE, "NEWLINE"},
        {NUMBER, "NUMBER"},
        {STRING, "STRING"},
        {FUNCTION, "FUNCTION"},
        {RETURN, "RETURN"},
        {LOOP, "LOOP"},
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
        throw runtime_error("Unexpected end of input - expected: " + token_type_to_string(token_type));
    }

    if (look_ahead_->type != token_type) {
        throw runtime_error("Unexpected token: " + token_type_to_string(look_ahead_->type) + " at (" + to_string(look_ahead_->line) + ", " + to_string(look_ahead_->column) + ") - expected: " + token_type_to_string(token_type));
    }

    Token token = look_ahead_.value();
    look_ahead_ = lexer_.get_next_token();
    return token;
}

bool Parser::match(const TokenType& token_type) {
    return look_ahead_->type == token_type;
}

bool Parser::is_integer(const string& lexeme) {
    if (lexeme.find('.') != string::npos) {
        return false;
    }
    return true;
}

