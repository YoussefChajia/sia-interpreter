// #pragma once
//
// #include <memory>
// #include <string>
// #include <optional>
// #include "lexer.hpp"
//
// using namespace std;
//
// class Node {
// public:
//     Node() = default;
//     string type;
//     virtual ~Node() = default;
// };
//
// // ==================================================
//
// class Statement : public Node {
// public:
//     Statement() = default;
//     virtual ~Statement() = default;
// };
//
// class Expression : public Node {
// public:
//     Expression() = default;
//     virtual ~Expression() = default;
// };
//
// // ==================================================
//
// class BlockStatement : public Statement {
// public:
//     explicit BlockStatement(const vector<shared_ptr<Statement>>& bd) : body(bd) {
//         type = "BlockStatement";
//     }
//     vector<shared_ptr<Statement>> body;
//     virtual ~BlockStatement() = default;
// };
//
// class ExpressionStatement : public Statement {
// public:
//     explicit ExpressionStatement(const shared_ptr<Expression>& exp) : expression(exp) {
//         type = "ExpressionStatement";
//     }
//     shared_ptr<Expression> expression;
//     virtual ~ExpressionStatement() = default;
// };
//
// // ==================================================
//
// class Literal : public Expression {
// public:
//     Literal() = default;
//     virtual ~Literal() = default;
// };
//
// class Identifier : public Expression {
// public:
//     explicit Identifier(const string& val) : value(val) {
//         type = "Identifier";
//     }
//     string value;
//     virtual ~Identifier() = default;
// };
//
// class AssignmentExpression : public Expression {
// public:
//     explicit AssignmentExpression(const string& name, const shared_ptr<Expression>& exp) : identifier(name), expression(exp) {
//         type = "AssignmentExpression";
//     }
//     string identifier;
//     shared_ptr<Expression> expression;
//     virtual ~AssignmentExpression() = default;
// };
//
// // ==================================================
//
// class StringLiteral : public Literal {
// public:
//     explicit StringLiteral(const string& v) : value(v) {
//         type = "StringLiteral";
//     }
//     string value;
//     virtual ~StringLiteral() = default;
// };
//
// class NumericLiteral : public Literal {
// public:
//     explicit NumericLiteral(const double& v) : value(v) {
//         type = "NumericLiteral";
//     }
//     double value;
//     virtual ~NumericLiteral() = default;
// };
//
// // ==================================================
//
// class Program : public Node {
// public:
//     explicit Program(const vector<shared_ptr<Statement>>& b) : body(b) {
//         type = "Program";
//     }
//     vector<shared_ptr<Statement>> body;
//     virtual ~Program() = default;
// };
//
// // ==================================================
//
// class Parser {
// public:
//     Parser();
//     shared_ptr<Program> parse(const string& input);
//     virtual ~Parser() = default;
//
//     void print_ast(const shared_ptr<Node>& node, int indent = 0);
//
// private:
//     string input_;
//     Lexer lexer_;
//     optional<Token> look_ahead_;
//
//     vector<shared_ptr<Statement>> parse_statement_list();
//     shared_ptr<Statement> parse_statement();
//
//     shared_ptr<BlockStatement> parse_block_statement();
//     shared_ptr<ExpressionStatement> parse_expression_statement();
//     shared_ptr<Expression> parse_expression();
//
//     shared_ptr<Expression> parse_identifier();
//
//     shared_ptr<StringLiteral> parse_string_literal();
//     shared_ptr<NumericLiteral> parse_numeric_literal();
//
//     Token eat(const TokenType& token_type);
//     string token_type_to_string(const TokenType& token_type);
// };

#pragma once

#include <memory>
#include <string>
#include <optional>
#include <vector>
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
    unique_ptr<StatementNode> parse_assignment();

    unique_ptr<ExpressionNode> parse_expression();
    unique_ptr<ExpressionNode> parse_term();
    unique_ptr<ExpressionNode> parse_factor();
    unique_ptr<ExpressionNode> parse_primary();

    unique_ptr<StatementNode> parse_print();
    vector<unique_ptr<ExpressionNode>> parse_print_args();

    Token eat(const TokenType& token_type);
    bool match(const TokenType& token_type);
    string token_type_to_string(const TokenType& token_type);
};
