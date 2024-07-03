#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "token.h"
#include "ASTNode.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class Parser {
private:
    vector<Token> tokens;
    size_t current = 0;

    unique_ptr<ASTNode> parseStatement();
    unique_ptr<ASTNode> parseAssignment();
    unique_ptr<ASTNode> parseIfStatement();
    unique_ptr<ASTNode> parseWhileStatement();
    unique_ptr<ASTNode> parseExpression();
    unique_ptr<ASTNode> parseLogicalExpr();
    unique_ptr<ASTNode> parseComparisonExpr();
    unique_ptr<ASTNode> parseArithmeticExpr();
    unique_ptr<ASTNode> parseTerm();
    unique_ptr<ASTNode> parseFactor();
    unique_ptr<ASTNode> parseComparisonOp();

    Token advance();
    Token peek();
    bool isAtEnd();
    bool match(TokenType type);
    bool match(vector<TokenType> types);

public:
    Parser(const vector<Token>& tokens);
    json parse();
    unique_ptr<Program> parseProgram();
};

#endif // PARSER_H