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
            out += this->variant_to_string(argument, line, column) + " ";
        }
        if (!out.empty()) out.pop_back();
        cout << out << endl;

        return monostate();
    };

    native_functions_["pow"] = [this](const vector<my_variant>& arguments, unsigned int line, unsigned int column) {
        if (arguments.size() != 2) {
            throw runtime_error(error_message("pow funciton requires exactly 2 arguments: base and exponent", line, column));
        }

        double base = to_double(arguments[0], line, column);
        double exponent = to_double(arguments[1], line, column);

        return pow(base, exponent);
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

void Evaluator::evaluate_block(const BlockNode& block, bool new_scope) {
    if (new_scope) push_scope();
    try {
        for (const auto& statement : block.statements) {
            evaluate_statement(*statement);
        }
    } catch (const return_exception& my_return) {
        if (new_scope) pop_scope();
        // throwing the exception that was caught
        throw;
    } catch (const runtime_error& e) {
        pop_scope();
        throw runtime_error(error_message("Error inside block", block.line, block.column));
    }
    if (new_scope) pop_scope();
}

void Evaluator::evaluate_statement(const StatementNode& statement) {
    // returns either a valid or null pointer
    if (auto block = dynamic_cast<const BlockNode*>(&statement)) {
        evaluate_block(*block, true);

    } else if (auto assignment = dynamic_cast<const AssignmentNode*>(&statement)) {
        my_variant value = evaluate_expression(*assignment->expression);
        set_variable(assignment->identifier, value);

    } else if (auto loop = dynamic_cast<const LoopNode*>(&statement)) {
        evaluate_loop(*loop);

    } else if (auto if_else = dynamic_cast<const IfElseNode*>(&statement)) {
        evaluate_if_else(*if_else);

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

        return native_it->second(arguments, call.line, call.column);
    }

    auto it = functions_.find(call.name);
    if (it == functions_.end()) {
        throw runtime_error(error_message("Undefined function : " + call.name, call.line, call.column));
    }

    const auto& function = it->second;
    if (call.arguments.size() != function.parameters.size()) {
        throw runtime_error(error_message("Argument count mismatch", call.line, call.column));
    }

    push_scope();

    try {
        vector<my_variant> evaluated_args;
        for (const auto& arg : call.arguments) {
            evaluated_args.push_back(evaluate_expression(*arg));
        }

        for (size_t i = 0; i < evaluated_args.size(); ++i) {
            set_variable(function.parameters[i], evaluated_args[i]);
        }

        evaluate_block(*function.body, false);
    } catch (const return_exception& my_return) {
        pop_scope();
        return my_return.value;
    }

    pop_scope();
    return monostate();
}

void Evaluator::evaluate_loop(const LoopNode& loop) {
    my_variant expression = evaluate_expression(*loop.condition);
    if (long number = to_long(expression, loop.line, loop.column)) {
        for (auto i = 0; i < number; ++i) {
            evaluate_block(*loop.body, false);
        }
    } else if (bool *condition = get_if<bool>(&expression)) {
        while (*condition) {
            evaluate_block(*loop.body, false);
        }
    }
}

void Evaluator::evaluate_if_else(const IfElseNode& if_else) {
    my_variant expression = evaluate_expression(*if_else.condition);
    bool condition = to_boolean(expression, if_else.line, if_else.column);
    if (condition) {
        evaluate_block(*if_else.if_branch, false);
    } else if (if_else.else_branch) {
        evaluate_block(*if_else.else_branch, false);
    }
}

my_variant Evaluator::evaluate_expression(const ExpressionNode& expression) {
    if (auto string = dynamic_cast<const StringLiteral*>(&expression)) {
        return my_variant(string->value);

    } else if (auto number = dynamic_cast<const LongNumberLiteral*>(&expression)) {
        return my_variant(number->value);

    } else if (auto number = dynamic_cast<const DoubleNumberLiteral*>(&expression)) {
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
    switch (op) {
        // for long, doubles and booleans
        case TokenType::LOGICAL_OR :
        case TokenType::LOGICAL_AND : {
            bool left_bool = to_boolean(left, line, column);
            bool right_bool = to_boolean(right, line, column);
            return (op == TokenType::LOGICAL_OR) ? (left_bool || right_bool) : (left_bool && right_bool);
        }

        // for long and doubles
        case TokenType::LESS_THAN :
        case TokenType::GREATER_THAN :
        case TokenType::LESS_EQUAL :
        case TokenType::GREATER_EQUAL : {
            double left_double = to_double(left, line, column);
            double right_double = to_double(right, line, column);
            switch (op) {
                case TokenType::LESS_THAN : return left_double < right_double;
                case TokenType::GREATER_THAN : return left_double > right_double;
                case TokenType::LESS_EQUAL :  return left_double <= right_double;
                case TokenType::GREATER_EQUAL :  return left_double >= right_double;
                default: throw runtime_error(error_message("Expected a number", line, column));
            }
        }
        // for long, double, string and booleans
        case TokenType::EQUAL : return are_equal(left, right, line, column);
        case TokenType::NOT_EQUAL : return !are_equal(left, right, line, column);

        // for long, double and strings
        case TokenType::PLUS : {
            if (holds_alternative<string>(left) || holds_alternative<string>(right)) {
                return variant_to_string(left, line, column) + variant_to_string(right, line, column);
            } else if (is_number(left) && is_number(right)) {
                if (holds_alternative<long>(left) && holds_alternative<long>(right)) {
                    return get<long>(left) + get<long>(right);
                } else {
                    return to_double(left, line, column) + to_double(right, line, column);
                }
            }
            throw runtime_error(error_message("Expected a string or a number", line, column));
        }
        // for long and doubles
        case TokenType::MINUS :
        case TokenType::MULTIPLY :
        case TokenType::DIVIDE : {
            double left_double = to_double(left, line, column);
            double right_double = to_double(right, line, column);
            switch (op) {
                case TokenType::MINUS : return left_double - right_double;
                case TokenType::MULTIPLY : return left_double * right_double;
                case TokenType::DIVIDE : {
                    if (right_double == 0) throw runtime_error(error_message("Division by zero", line, column));
                    return left_double / right_double;
                }
                default: throw runtime_error(error_message("Expected a number", line, column));
            }
        }
        // for longs
        case TokenType::MODULO : {
            if (holds_alternative<long>(left) && holds_alternative<long>(right)) {
                long right_long = get<long>(right);
                if (right_long == 0) throw runtime_error(error_message("Division by zero", line, column));
                return get<long>(left) % right_long;
            }
            throw runtime_error(error_message("Modulo requires integers", line, column));
        }
        default: throw runtime_error(error_message("Invallid operator", line, column));
    }
}

my_variant Evaluator::evaluate_unary_op(TokenType op, const my_variant& operand, unsigned int line, unsigned int column) {
    if (op == TokenType::MINUS) {
        if (holds_alternative<long>(operand)) return -get<long>(operand);
        if (holds_alternative<double>(operand)) return -get<double>(operand);
        throw runtime_error(error_message("Expected a number", line, column));
    }
    throw runtime_error(error_message("Invalid unary operator", line, column));
}

bool Evaluator::is_number(const my_variant& value) {
    return holds_alternative<long>(value) || holds_alternative<double>(value);
}

double Evaluator::to_double(const my_variant& value, unsigned int line, unsigned int column) {
    if (holds_alternative<long>(value)) return static_cast<double>(get<long>(value));
    if (holds_alternative<double>(value)) return get<double>(value);
    throw runtime_error(error_message("Expected a number", line, column));
}

long Evaluator::to_long(const my_variant& value, unsigned int line, unsigned int column) {
    if (holds_alternative<long>(value)) return get<long>(value);
    if (holds_alternative<double>(value)) throw runtime_error(error_message("Expected an integer", line, column));
    throw runtime_error(error_message("Expected a number", line, column));
}

bool Evaluator::are_equal(const my_variant&left, const my_variant&right, unsigned int line, unsigned int column) {
    if (holds_alternative<string>(left) && holds_alternative<string>(right)) return get<string>(left) == get<string>(right);
    if (is_number(left) && is_number(right)) return to_double(left, line, column) == to_double(right, line, column);
    if (holds_alternative<bool>(left) && holds_alternative<bool>(right)) return get<bool>(left) == get<bool>(right);
    throw runtime_error(error_message("Unexpected types of operands", line, column));
}

bool Evaluator::to_boolean(const my_variant& value, unsigned int line, unsigned int column) {
    if (holds_alternative<bool>(value)) return get<bool>(value);
    if (holds_alternative<long>(value)) return get<long>(value) != 0;
    if (holds_alternative<double>(value)) return get<double>(value) != 0.0;
    throw runtime_error(error_message("Expected a boolean or a number", line, column));
}

string Evaluator::variant_to_string(const my_variant& value, unsigned int line, unsigned int column) {
    if (holds_alternative<string>(value)) return get<string>(value);
    if (holds_alternative<long>(value)) return to_string(get<long>(value));
    if (holds_alternative<double>(value)) {
        string str = to_string(get<double>(value));
        str.erase(str.find_last_not_of('0') + 1, string::npos);
        if (str.back() == '.') str.pop_back();
        return str;
    }
    if (holds_alternative<bool>(value)) return get<bool>(value) ? "true" : "false";
    if (holds_alternative<monostate>(value)) return "null";
    throw runtime_error(error_message("Cannot convert to string", line, column));
}

string Evaluator::error_message(const string& message, unsigned int line, unsigned int column) {
    // safely constructing the string
    stringstream string_stream;
    string_stream << "Error at " << line << ", " << column << " : " << message;
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
