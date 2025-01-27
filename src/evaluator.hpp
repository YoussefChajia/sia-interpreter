#pragma once

#include <memory>
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

using namespace std;

struct function_value {
    vector<string> parameters;
    vector<unique_ptr<StatementNode>> body;
};

// current possible types in the language
using my_variant = variant<double, string>;

class Evaluator {
public:
    Evaluator();
    void evaluate(const ProgramNode& program);
    virtual ~Evaluator();

private:
    unordered_map<string, my_variant> symbol_table_;
    vector<unordered_map<string, my_variant>> scopes_;

    void push_scope() { scopes_.push_back(unordered_map<string, my_variant>()); }
    void pop_scope() { if (scopes_.size() > 1) scopes_.pop_back(); }

    my_variant get_variable(const string& name);
    void set_variable(const string& name, const my_variant& value);

    void evaluate_block(const BlockNode& block);
    void evaluate_statement(const StatementNode& statement);

    my_variant evaluate_expression(const ExpressionNode& expression);
    my_variant evaluate_binary_op(TokenType op, const my_variant& left, const my_variant& right, unsigned int line, unsigned int column);
    my_variant evaluate_unary_op(TokenType op, const my_variant& operand, unsigned int line, unsigned int column);

    double get_number(const my_variant& value, unsigned int line, unsigned int column);
    string variant_to_string(const my_variant& value);

    string error_message(const string& message, unsigned int line, unsigned int column);
};
