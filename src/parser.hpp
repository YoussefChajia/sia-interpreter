#pragma once

#include <memory>
#include <string>
#include <optional>
#include "lexer.hpp"

using namespace std;

class Node {
public:
    Node() = default;
    string type;
    virtual ~Node() = default;
};

/*  
 *  Program
 *    : StatementList
 *    ;
 *
 *  StatementList
 *    : Statement
 *    | StatementList
 *    ;
 *
 *  Statement
 *    : ExpressionStatement
 *    | BlockStatement
 *    ;
 *
 *  ExpressionStatement
 *    : Expression ';'
 *    ;
 *
 *  Literal
 *    : NumericLiteral
 *    | StringLiteral
 *    ;
 *
 */

class Expression : public Node {
public:
    Expression() = default;
    virtual ~Expression() = default;
};

class ExpressionStatement : public Node {
public:
    explicit ExpressionStatement(shared_ptr<Expression> ex) : expression(ex) {
        type = "ExpressionStatement";
    }
    shared_ptr<Expression> expression;
    virtual ~ExpressionStatement() = default;
};

class Literal : public Expression {
public:
    Literal() = default;
    virtual ~Literal() = default;
};

class StringLiteral : public Literal {
public:
    explicit StringLiteral(const string& v) : value(v) {
        type = "StringLiteral";
    }
    string value;
    virtual ~StringLiteral() = default;
};

class NumericLiteral : public Literal {
public:
    explicit NumericLiteral(const double& v) : value(v) {
        type = "NumericLiteral";
    }
    double value;
    virtual ~NumericLiteral() = default;
};

class Program : public Node {
public:
    explicit Program(const vector<shared_ptr<ExpressionStatement>>& b) : body(b) {
        type = "Program";
    }
    vector<shared_ptr<ExpressionStatement>> body;
    virtual ~Program() = default;
};


class Parser {
public:
    Parser(const string& input);
    shared_ptr<Program> parse();
    virtual ~Parser() = default;

    void print_ast(const shared_ptr<Node>& node, int indent = 0);

private:
    string input_;
    Lexer lexer_;
    optional<Token> look_ahead_;

    vector<shared_ptr<ExpressionStatement>> parse_statement_list();
    shared_ptr<ExpressionStatement> parse_statement();
    shared_ptr<ExpressionStatement> parse_expression_statement();
    shared_ptr<Expression> parse_expression();
    shared_ptr<Literal> parse_literal();
    shared_ptr<StringLiteral> parse_string_literal();
    shared_ptr<NumericLiteral> parse_numeric_literal();
    Token eat(const TokenType& token_type);
    string token_type_to_string(const TokenType& token_type);

    void print_indent(int indent);
    void print_comma(bool last_item);
};
