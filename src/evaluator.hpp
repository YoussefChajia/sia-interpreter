#pragma once

#include <exception>
#include <functional>
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

using namespace std;


// current possible types in the language
using my_variant = variant<long, double, string, bool, monostate>;
using native_function = function<my_variant(const vector<my_variant>&, unsigned int line, unsigned int column)>;

class Evaluator {
public:
    Evaluator();
    void evaluate(const ProgramNode& program);
    virtual ~Evaluator();

private:
    struct function_def {
        vector<string> parameters;
        const BlockNode* body;
    };

    struct return_exception : exception {
        my_variant value;
        explicit return_exception(my_variant value) : value(std::move(value)) {}
    };

    unordered_map<string, my_variant> symbol_table_;
    vector<unordered_map<string, my_variant>> scopes_;
    unordered_map<string, function_def> functions_;

    unordered_map<string, native_function> native_functions_;

    void push_scope() { scopes_.push_back(unordered_map<string, my_variant>()); }
    void pop_scope() { if (!scopes_.empty()) scopes_.pop_back(); }

    my_variant get_variable(const string& name);
    void set_variable(const string& name, const my_variant& value);

    void evaluate_block(const BlockNode& block, bool new_scope);
    void evaluate_statement(const StatementNode& statement);
    my_variant evaluate_function_call(const FunctionCallNode& call);
    void evaluate_expression_statment(const ExpressionStatementNode& expression_statement);

    void evaluate_loop(const LoopNode& loop);
    void evaluate_if_else(const IfElseNode& if_else);

    my_variant evaluate_expression(const ExpressionNode& expression);
    my_variant evaluate_binary_op(TokenType op, const my_variant& left, const my_variant& right, unsigned int line, unsigned int column);
    my_variant evaluate_unary_op(TokenType op, const my_variant& operand, unsigned int line, unsigned int column);

    bool is_number(const my_variant& value);
    double to_double(const my_variant& value, unsigned int line, unsigned int column);
    long to_long(const my_variant& value, unsigned int line, unsigned int column);
    bool to_boolean(const my_variant& value, unsigned int line, unsigned int column);
    bool are_equal(const my_variant&left, const my_variant&right, unsigned int line, unsigned int column);
    string variant_to_string(const my_variant& value, unsigned int line, unsigned int column);

    string error_message(const string& message, unsigned int line, unsigned int column);
};
