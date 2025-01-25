#pragma once

#include <unordered_map>
#include <variant>
#include <string>

#include "ast.hpp"
#include "token.hpp"

using namespace std;
using my_variant = variant<double, string>;

class Evaluator {
public:
    Evaluator();
    void evaluate(const ProgramNode& program);

private:
    unordered_map<string, my_variant> symbol_table_;

    void evaluate_statement(const StatementNode& statement);
    my_variant evaluate_expression(const ExpressionNode& expression);
    my_variant evaluate_binary_op(TokenType op, const my_variant& left, const my_variant& right, unsigned int line, unsigned int column);
    my_variant evaluate_unary_op(TokenType op, const my_variant& operand, unsigned int line, unsigned int column);
    double get_number(const my_variant& value, unsigned int line, unsigned int column);
    string variant_to_string(const my_variant& value);
    string error_message(const string& message, unsigned int line, unsigned int column);
};
