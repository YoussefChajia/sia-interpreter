#include <iostream>

#include "ast.hpp"
#include "evaluator.hpp"


using namespace std;
using my_variant = variant<double, string>;

Evaluator::Evaluator() {}

void Evaluator::evaluate(const ProgramNode& program) {
    for (const auto& statement : program.statements) {
        evaluate_statement(*statement);
    }
}

void Evaluator::evaluate_statement(const StatementNode& statement) {
    // returns either a valid or null pointer
    if (auto assignment = dynamic_cast<const AssignmentNode*>(&statement)) {
        my_variant value = evaluate_expression(*assignment->expression);
        symbol_table_[assignment->identifier] = value;
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
my_variant Evaluator::evaluate_expression(const ExpressionNode& expression) {}
my_variant Evaluator::evaluate_binary_op(TokenType op, const my_variant& left, const my_variant& right, unsigned int line, unsigned int column) {}
my_variant Evaluator::evaluate_unary_op(TokenType op, const my_variant& operand, unsigned int line, unsigned int column) {}
double Evaluator::get_number(const my_variant& value, unsigned int line, unsigned int column) {}
string Evaluator::variant_to_string(const my_variant& value) {}
string Evaluator::error_message(const string& message, unsigned int line, unsigned int column) {}
