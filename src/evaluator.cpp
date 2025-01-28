#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "ast.hpp"
#include "token.hpp"
#include "evaluator.hpp"


using namespace std;

Evaluator::Evaluator() {
    push_scope();
}

Evaluator::~Evaluator() {
    pop_scope();
}

void Evaluator::evaluate(const ProgramNode& program) {
    for (const auto& statement : program.statements) {
        evaluate_statement(*statement);
    }
}

void Evaluator::evaluate_block(const BlockNode& block) {
    push_scope();
    try {
        for (const auto& statement : block.statements) {
            evaluate_statement(*statement);
        }
    } catch (const runtime_error& e) {
        pop_scope();
        throw runtime_error(error_message("Error inside block", block.line, block.column));
    }
    pop_scope();
}

void Evaluator::evaluate_statement(const StatementNode& statement) {
    // returns either a valid or null pointer
    if (auto block = dynamic_cast<const BlockNode*>(&statement)) {
        evaluate_block(*block);
    } else if (auto function_def = dynamic_cast<const FunctionDefNode*>(&statement)) {
        functions_[function_def->name] = {
            function_def->parameters,
            function_def->body.get()
        };
    } else if (auto assignment = dynamic_cast<const AssignmentNode*>(&statement)) {
        my_variant value = evaluate_expression(*assignment->expression);
        set_variable(assignment->identifier, value);
    } else if (auto function_call = dynamic_cast<const FunctionCallNode*>(&statement)) {
        evaluate_function_call(*function_call);
    } else if (auto print = dynamic_cast<const PrintNode*>(&statement)) {
        string out;
        for (const auto& argument : print->arguments) {
            my_variant value = evaluate_expression(*argument);
            out += this->variant_to_string(value) + " ";
        }
        if (!out.empty()) out.pop_back();
        cout << out << endl;
    } else {
        throw runtime_error("Unknown statement");
    }
}

void Evaluator::evaluate_function_call(const FunctionCallNode& call) {
    auto it = functions_.find(call.name);
    if (it == functions_.end()) {
        throw runtime_error(error_message("Undefined function : " + call.name, call.line, call.column));
    }

    const auto& function = it->second;
    if (call.arguments.size() != function.parameters.size()) {
        throw runtime_error(error_message("Argument count mismatch", call.line, call.column));
    }

    vector<my_variant> args;
    for (const auto& arg : call.arguments) {
        args.push_back(evaluate_expression(*arg));
    }

    push_scope();

    for (size_t i = 0; i < args.size(); ++i) {
        set_variable(function.parameters[i], args[i]);
    }

    evaluate_block(*function.body);

    pop_scope();
}

my_variant Evaluator::evaluate_expression(const ExpressionNode& expression) {
    if (auto string = dynamic_cast<const StringLiteral*>(&expression)) {
        return my_variant(string->value);
    } else if (auto number = dynamic_cast<const NumberLiteral*>(&expression)) {
        return my_variant(number->value);
    } else if (auto var = dynamic_cast<const VariableNode*>(&expression)) {
        return get_variable(var->identifier);
    } else if (auto binary = dynamic_cast<const BinaryOpNode*>(&expression)) {
        my_variant left = evaluate_expression(*binary->left);
        my_variant right = evaluate_expression(*binary->right);
        return evaluate_binary_op(binary->op, left, right, binary->line, binary->column);
    } else if (auto unary = dynamic_cast<const UnaryOpNode*>(&expression)) {
        my_variant operand = evaluate_expression(*unary->operand);
        return evaluate_unary_op(unary->op, operand, unary->line, unary->column);
    } else {
        throw runtime_error("Unknown expression");
    }
}

my_variant Evaluator::evaluate_binary_op(TokenType op, const my_variant& left, const my_variant& right, unsigned int line, unsigned int column) {
    double left_number = get_number(left, line, column);
    double right_number = get_number(right, line, column);
    switch (op) {
        case TokenType::PLUS : return left_number + right_number;
        case TokenType::MINUS : return left_number - right_number;
        case TokenType::MULTIPLY : return left_number * right_number;
        case TokenType::DIVIDE : 
            if (right_number == 0) throw runtime_error(error_message("Division by zero", line, column));
            return left_number / right_number;
        default: throw runtime_error(error_message("Invallid operator", line, column));
    }
}

my_variant Evaluator::evaluate_unary_op(TokenType op, const my_variant& operand, unsigned int line, unsigned int column) {
    if (op == TokenType::MINUS) return -get_number(operand, line, column);
    throw runtime_error(error_message("Invallid unary operator", line, column));
}

double Evaluator::get_number(const my_variant& value, unsigned int line, unsigned int column) {
    if (holds_alternative<double>(value)) return get<double>(value);
    throw runtime_error(error_message("Expected number", line, column));
}

string Evaluator::variant_to_string(const my_variant& value) {
    if (holds_alternative<double>(value)) {
        string str = to_string(get<double>(value));
        str.erase(str.find_last_not_of('0') + 1, string::npos);
        if (str.back() == '.') str.pop_back();
        return str;
    }
    return get<string>(value);
}

string Evaluator::error_message(const string& message, unsigned int line, unsigned int column) {
    // safely constructing the string
    stringstream string_stream;
    cout << "Error at " << line << ":" << column << " - " << message;
    return string_stream.str();
}

my_variant Evaluator::get_variable(const string& name) {
    for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
        auto var = scope->find(name);
        if (var != scope->end()) {
            return var->second;
        }
    }
    throw runtime_error("Undefined variable " + name);
}

void Evaluator::set_variable(const string& name, const my_variant& value) {
    scopes_.back()[name] = value;
}
