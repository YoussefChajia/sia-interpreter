#include "../headers/interpreter.h"
#include <iostream>
#include <stdexcept>

void Interpreter::interpret(const unique_ptr<Program>& program) {
    try {
        for (const auto& stmt : program->body) {
            evaluate(stmt);
        }
    } catch (const runtime_error& e) {
        cerr << "Runtime error: " << e.what() << endl;
    }
}

void Interpreter::evaluate(const unique_ptr<ASTNode>& node) {
    if (auto assignment = dynamic_cast<AssignmentStatement*>(node.get())) {
        auto variable = assignment->variable;
        auto valueNode = assignment->expression.get();

        if (auto literal = dynamic_cast<Literal*>(valueNode)) {
            symbolTable[variable] = make_shared<Literal>(*literal);
        } else if (auto binaryExpr = dynamic_cast<BinaryExpression*>(valueNode)) {
            auto leftValue = evaluateExpression(binaryExpr->left);
            auto rightValue = evaluateExpression(binaryExpr->right);
            auto result = evaluateBinaryExpression(leftValue, rightValue, binaryExpr->op);
            symbolTable[variable] = make_shared<Literal>(result, "Number");
        } else if (auto unaryExpr = dynamic_cast<UnaryExpression*>(valueNode)) {
            auto value = evaluateExpression(unaryExpr->operand);
            string result;
            if (unaryExpr->op == "-") {
                result = to_string(-stoi(value));
            } else if (unaryExpr->op == "not") {
                result = (value == "true" ? "false" : "true");
            }
            symbolTable[variable] = make_shared<Literal>(result, "Boolean");
        } else if (auto variableExpr = dynamic_cast<Variable*>(valueNode)) {
            if (symbolTable.find(variableExpr->name) != symbolTable.end()) {
                symbolTable[variable] = symbolTable[variableExpr->name];
            } else {
                throw runtime_error("Undefined variable: " + variableExpr->name);
            }
        } else {
            throw runtime_error("Unsupported expression type in assignment.");
        }
    } else if (auto ifStmt = dynamic_cast<IfStatement*>(node.get())) {
        evaluateIfStatement(ifStmt);
    } else if (auto whileStmt = dynamic_cast<WhileStatement*>(node.get())) {
        evaluateWhileStatement(whileStmt);
    } else {
        throw runtime_error("Unsupported statement type.");
    }
}

void Interpreter::evaluateIfStatement(IfStatement* ifStmt) {
    auto conditionValue = evaluateExpression(ifStmt->condition);
    bool condition = (conditionValue == "true");

    if (condition) {
        evaluate(ifStmt->thenBranch);
    } else if (ifStmt->elseBranch) {
        evaluate(ifStmt->elseBranch);
    }
}

void Interpreter::evaluateWhileStatement(WhileStatement* whileStmt) {
    while (evaluateExpression(whileStmt->condition) == "true") {
        evaluate(whileStmt->body);
    }
}

string Interpreter::evaluateExpression(const unique_ptr<ASTNode>& node) {
    if (auto literal = dynamic_cast<Literal*>(node.get())) {
        return literal->value;
    } else if (auto variable = dynamic_cast<Variable*>(node.get())) {
        if (symbolTable.find(variable->name) != symbolTable.end()) {
            auto valueNode = symbolTable[variable->name];
            return dynamic_cast<Literal*>(valueNode.get())->value;
        } else {
            throw runtime_error("Undefined variable: " + variable->name);
        }
    } else if (auto binaryExpr = dynamic_cast<BinaryExpression*>(node.get())) {
        auto leftValue = evaluateExpression(binaryExpr->left);
        auto rightValue = evaluateExpression(binaryExpr->right);
        return evaluateBinaryExpression(leftValue, rightValue, binaryExpr->op);
    } else if (auto unaryExpr = dynamic_cast<UnaryExpression*>(node.get())) {
        auto value = evaluateExpression(unaryExpr->operand);
        if (unaryExpr->op == "-") {
            return to_string(-stoi(value));
        } else if (unaryExpr->op == "not") {
            return (value == "true" ? "false" : "true");
        }
    }
    throw runtime_error("Unsupported expression type.");
}

string Interpreter::evaluateBinaryExpression(const string& left, const string& right, const string& op) {
    if (op == "+") {
        if (isdigit(left[0]) && isdigit(right[0])) {
            return to_string(stoi(left) + stoi(right));
        } else {
            throw runtime_error("Type error: cannot add non-numeric values.");
        }
    } else if (op == "-") {
        return to_string(stoi(left) - stoi(right));
    } else if (op == "*") {
        return to_string(stoi(left) * stoi(right));
    } else if (op == "/") {
        return to_string(stoi(left) / stoi(right));
    } else if (op == "%") {
        return to_string(stoi(left) % stoi(right));
    } else {
        throw runtime_error("Unsupported binary operator: " + op);
    }
}
