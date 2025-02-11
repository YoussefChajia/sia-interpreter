#pragma once

#include <memory>
#include <string>
#include <optional>
#include "ast.hpp"
#include "lexer.hpp"

using namespace std;

class Parser {
public:
    Parser();
    unique_ptr<ProgramNode> parse(const string& input);
    virtual ~Parser() = default;

private:
    string input_;
    Lexer lexer_;
    optional<Token> look_ahead_;

    unique_ptr<ProgramNode> parse_program();

    unique_ptr<StatementNode> parse_statement();
    unique_ptr<StatementNode> parse_identifier();
    unique_ptr<StatementNode> parse_assignment(Token& identifier);
    unique_ptr<LoopNode> parse_loop();
    unique_ptr<FunctionCallNode> parse_function_call(Token& identifier);
    unique_ptr<ReturnNode> parse_return();
    unique_ptr<FunctionDefNode> parse_function_def();
    unique_ptr<BlockNode> parse_block();
    unique_ptr<IfElseNode> parse_if_else();

    unique_ptr<ExpressionNode> parse_expression();
    unique_ptr<ExpressionNode> parse_logical_or();
    unique_ptr<ExpressionNode> parse_logical_and();
    unique_ptr<ExpressionNode> parse_comparison();
    unique_ptr<ExpressionNode> parse_term();
    unique_ptr<ExpressionNode> parse_factor();
    unique_ptr<ExpressionNode> parse_primary();

    Token eat(const TokenType& token_type);
    bool match(const TokenType& token_type);
    bool is_integer(const string& lexeme);
    string token_type_to_string(const TokenType& token_type);
};
