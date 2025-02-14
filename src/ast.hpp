#pragma once

#include "token.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace std;

class ASTNode {
public:
    unsigned int line, column;
    virtual ~ASTNode() = default;
};

class StatementNode : public ASTNode {};
class ExpressionNode : public ASTNode {};

class ProgramNode : public ASTNode {
public:
    vector<unique_ptr<StatementNode>> statements;

    virtual ~ProgramNode() = default;
};

class BlockNode : public StatementNode {
public:
    vector<unique_ptr<StatementNode>> statements;

    explicit BlockNode(vector<unique_ptr<StatementNode>> statements, unsigned int line, unsigned int column)
        : statements(std::move(statements)) {
            this->line = line;
            this->column = column;
        }

    virtual ~BlockNode() = default;
};

class AssignmentNode : public StatementNode {
public:
    string identifier;
    unique_ptr<ExpressionNode> expression;

    explicit AssignmentNode(string identifier, unique_ptr<ExpressionNode> expression, unsigned int line, unsigned int column)
        : identifier(std::move(identifier)), expression(std::move(expression)) {
            this->line = line;
            this->column = column;
        }
    
    virtual ~AssignmentNode() = default;
};

class BinaryOpNode : public ExpressionNode {
public:
    TokenType op;
    unique_ptr<ExpressionNode> left;
    unique_ptr<ExpressionNode> right;

    explicit BinaryOpNode(TokenType op, unique_ptr<ExpressionNode> left, unique_ptr<ExpressionNode> right, unsigned int line, unsigned int column)
        : op(op), left(std::move(left)), right(std::move(right)) {
            this->line = line;
            this->column = column;
        }

    virtual ~BinaryOpNode() = default;
};

class UnaryOpNode : public ExpressionNode {
public:
    TokenType op;
    unique_ptr<ExpressionNode> operand;

    explicit UnaryOpNode(TokenType op, unique_ptr<ExpressionNode> operand, unsigned int line, unsigned int column)
        : op(op), operand(std::move(operand)) {
            this->line = line;
            this->column = column;
        }

    virtual ~UnaryOpNode() = default;
};

class VariableNode : public ExpressionNode {
public:
    string identifier;

    explicit VariableNode(string identifier, unsigned int line, unsigned int column)
        : identifier(std::move(identifier)) {
            this->line = line;
            this->column = column;
        }

    virtual ~VariableNode() = default;
};

class LiteralNode : public ExpressionNode {};

class StringLiteral : public LiteralNode {
public:
    string value;

    explicit StringLiteral(string value, unsigned int line, unsigned int column)
        : value(std::move(value)) {
            this->line = line;
            this->column = column;
        }
    virtual ~StringLiteral() = default;
};

class LongNumberLiteral : public LiteralNode {
public:
    long value;

    explicit LongNumberLiteral(long value, unsigned int line, unsigned int column)
        : value(value) {
            this->line = line;
            this->column = column;
        }
    virtual ~LongNumberLiteral() = default;
};

class DoubleNumberLiteral : public LiteralNode {
public:
    double value;

    explicit DoubleNumberLiteral(double value, unsigned int line, unsigned int column)
        : value(value) {
            this->line = line;
            this->column = column;
        }
    virtual ~DoubleNumberLiteral() = default;
};

class BoolLiteral : public LiteralNode {
public:
    bool value;

    explicit BoolLiteral(bool value, unsigned int line, unsigned int column)
        : value(value) {
            this->line = line;
            this->column = column;
        }
    virtual ~BoolLiteral() = default;
};

class FunctionDefNode : public StatementNode {
public:
    string name;
    vector<string> parameters;
    unique_ptr<BlockNode> body;

    explicit FunctionDefNode(string name, vector<string> parameters, unique_ptr<BlockNode> body, unsigned int ln, unsigned int col)
        : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {
            line = ln;
            column = col;
        }

    virtual ~FunctionDefNode() = default;
};

class FunctionCallNode : public ExpressionNode {
public:
    string name;
    vector<unique_ptr<ExpressionNode>> arguments;

    explicit FunctionCallNode(string name, vector<unique_ptr<ExpressionNode>> arguments, unsigned int ln, unsigned int col)
        : name(std::move(name)), arguments(std::move(arguments)) {
            line = ln;
            column = col;
        }

    virtual ~FunctionCallNode() = default;
};

class ExpressionStatementNode : public StatementNode {
public:
    unique_ptr<ExpressionNode> expression;

    explicit ExpressionStatementNode(unique_ptr<ExpressionNode> expression, unsigned int line, unsigned int column)
        : expression(std::move(expression)) {
            this->line = line;
            this->column = column;
        }

    virtual ~ExpressionStatementNode() = default;
};

class ReturnNode : public StatementNode {
public:
    unique_ptr<ExpressionNode> expression;

    explicit ReturnNode(unique_ptr<ExpressionNode> expression, unsigned int line, unsigned int column)
        : expression(std::move(expression)) {
            this->line = line;
            this->column = column;
        }

    virtual ~ReturnNode() = default;
};

class LoopNode : public StatementNode {
public:
    unique_ptr<ExpressionNode> condition;
    unique_ptr<BlockNode> body;

    explicit LoopNode(unique_ptr<ExpressionNode> condition, unique_ptr<BlockNode> body, unsigned int ln, unsigned int col)
        : condition(std::move(condition)), body(std::move(body)) {
            line = ln;
            column = col;
        }

    virtual ~LoopNode() = default;
};

class IfElseNode : public StatementNode {
public:
    unique_ptr<ExpressionNode> condition;
    unique_ptr<BlockNode> if_branch;
    unique_ptr<BlockNode> else_branch;

    explicit IfElseNode(unique_ptr<ExpressionNode> condition, unique_ptr<BlockNode> if_branch, unique_ptr<BlockNode> else_branch, unsigned int ln, unsigned int col)
        : condition(std::move(condition)), if_branch(std::move(if_branch)), else_branch(std::move(else_branch)) {
        line = ln;
        column = col;
    }

    virtual ~IfElseNode() = default;
};

