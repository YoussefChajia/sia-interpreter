#ifndef ASTNODE_H
#define ASTNODE_H

#include <vector>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual json toJson() const = 0;
};

class Program : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> body;
    json toJson() const override;
};

class AssignmentStatement : public ASTNode {
public:
    string variable;
    unique_ptr<ASTNode> expression;
    json toJson() const override;
};

class BinaryExpression : public ASTNode {
public:
    string op;
    unique_ptr<ASTNode> left;
    unique_ptr<ASTNode> right;
    BinaryExpression(const string& op, unique_ptr<ASTNode> left, unique_ptr<ASTNode> right)
        : op(op), left(move(left)), right(move(right)) {}
    json toJson() const override;
};

class UnaryExpression : public ASTNode {
public:
    string op;
    unique_ptr<ASTNode> operand;
    UnaryExpression(const string& op, unique_ptr<ASTNode> operand)
        : op(op), operand(move(operand)) {}
    json toJson() const override;
};

class Literal : public ASTNode {
public:
    string value;
    string type;
    Literal(const string& value, const string& type) : value(value), type(type) {}
    json toJson() const override;
};

class Variable : public ASTNode {
public:
    string name;
    Variable(const string& name) : name(name) {}
    json toJson() const override;
};

class IfStatement : public ASTNode {
public:
    unique_ptr<ASTNode> condition;
    unique_ptr<ASTNode> thenBranch;
    unique_ptr<ASTNode> elseBranch;
    IfStatement(unique_ptr<ASTNode> condition, unique_ptr<ASTNode> thenBranch, unique_ptr<ASTNode> elseBranch)
        : condition(move(condition)), thenBranch(move(thenBranch)), elseBranch(move(elseBranch)) {}
    json toJson() const override;
};

class WhileStatement : public ASTNode {
public:
    unique_ptr<ASTNode> condition;
    unique_ptr<ASTNode> body;
    WhileStatement(unique_ptr<ASTNode> condition, unique_ptr<ASTNode> body)
        : condition(move(condition)), body(move(body)) {}
    json toJson() const override;
};

#endif // ASTNODE_H
