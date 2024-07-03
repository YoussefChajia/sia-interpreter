#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <memory>
#include <unordered_map>
#include <string>
#include "ASTNode.h"

using namespace std;

class Interpreter {
private:
    unordered_map<string, shared_ptr<ASTNode>> symbolTable;

    void evaluate(const unique_ptr<ASTNode>& node);
    string evaluateExpression(const unique_ptr<ASTNode>& node);
    string evaluateBinaryExpression(const string& left, const string& right, const string& op);
    void evaluateIfStatement(IfStatement* ifStmt);
    void evaluateWhileStatement(WhileStatement* whileStmt);

public:
    void interpret(const unique_ptr<Program>& program);
};

#endif // INTERPRETER_H
