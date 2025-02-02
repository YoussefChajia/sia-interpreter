#include <cmath>
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

    // [] -> captures variables to use inside the lambda function
    native_functions_["print"] = [this](const vector<my_variant>& arguments, unsigned int line, unsigned int column) {
        string out;
        for (const auto& argument : arguments) {
            out += this->variant_to_string(argument) + " ";
        }
        if (!out.empty()) out.pop_back();
        cout << out << endl;
    };

    native_functions_["pow"] = [this](const vector<my_variant>& arguments, unsigned int line, unsigned int column) {
        if (arguments.size() != 2) {
            throw runtime_error(error_message("pow funciton requires exactly 2 arguments: base and exponent", line, column));
        }

        double base = get_number(arguments[0], line, column);
        double exponent = get_number(arguments[1], line, column);

        double result = pow(base, exponent);

        throw return_exception(result);
    };
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

    } else if (auto assignment = dynamic_cast<const AssignmentNode*>(&statement)) {
        my_variant value = evaluate_expression(*assignment->expression);
        set_variable(assignment->identifier, value);

    } else if (auto loop = dynamic_cast<const LoopNode*>(&statement)) {
        evaluate_loop(*loop);

    } else if (auto function_def = dynamic_cast<const FunctionDefNode*>(&statement)) {
        functions_[function_def->name] = { function_def->parameters, function_def->body.get() };

    } else if (auto expression_statement = dynamic_cast<const ExpressionStatementNode*>(&statement)) {
        evaluate_expression_statment(*expression_statement);

    } else if (auto my_return = dynamic_cast<const ReturnNode*>(&statement)) {
        my_variant value = my_return->expression ? evaluate_expression(*my_return->expression) : my_variant(monostate());
        throw return_exception(value);

    } else {
        throw runtime_error("Unknown statement");
    }
}

void Evaluator::evaluate_expression_statment(const ExpressionStatementNode& expression_statement) {
    evaluate_expression(*expression_statement.expression);
}

my_variant Evaluator::evaluate_function_call(const FunctionCallNode& call) {
    auto native_it = native_functions_.find(call.name);
    if (native_it != native_functions_.end()) {
        vector<my_variant> arguments;
        for (const auto& argument  : call.arguments) {
            arguments.push_back(evaluate_expression(*argument));
        }

        try {
            native_it->second(arguments, call.line, call.column);
            return monostate();
        } catch (const return_exception& my_return) {
            pop_scope();
            return my_return.value;
        }
    }

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

    try {
        evaluate_block(*function.body);
    } catch (const return_exception& my_return) {
        pop_scope();
        return my_return.value;
    }

    pop_scope();
    return monostate();
}

void Evaluator::evaluate_loop(const LoopNode& loop) {
    my_variant expression = evaluate_expression(*loop.argument);
    push_scope();
    if (double *number = get_if<double>(&expression)) {
        for (auto i = 0; i < (int)*number; ++i) {
            evaluate_block(*loop.body);
        }
    } else if (bool *condition = get_if<bool>(&expression)) {
        while (*condition) {
            evaluate_block(*loop.body);
        }
    }
    pop_scope();
}

my_variant Evaluator::evaluate_expression(const ExpressionNode& expression) {
    if (auto string = dynamic_cast<const StringLiteral*>(&expression)) {
        return my_variant(string->value);

    } else if (auto number = dynamic_cast<const NumberLiteral*>(&expression)) {
        return my_variant(number->value);

    } else if (auto var = dynamic_cast<const VariableNode*>(&expression)) {
        return get_variable(var->identifier);

    } else if (auto bool_value = dynamic_cast<const BoolLiteral*>(&expression)) {
        return my_variant(bool_value->value);

    } else if (auto binary = dynamic_cast<const BinaryOpNode*>(&expression)) {
        my_variant left = evaluate_expression(*binary->left);
        my_variant right = evaluate_expression(*binary->right);
        return evaluate_binary_op(binary->op, left, right, binary->line, binary->column);

    } else if (auto unary = dynamic_cast<const UnaryOpNode*>(&expression)) {
        my_variant operand = evaluate_expression(*unary->operand);
        return evaluate_unary_op(unary->op, operand, unary->line, unary->column);

    } else if (auto function_call = dynamic_cast<const FunctionCallNode*>(&expression)) {
        return evaluate_function_call(*function_call);

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
