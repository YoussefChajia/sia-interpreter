#include "../headers/parser.h"
#include "../headers/ASTNode.h"
#include <stdexcept>
#include <unordered_map>

using namespace std;

string TokenTypeToString(TokenType type) {
    static unordered_map<TokenType, string> tokenTypeToString = {
        {TokenType::PLUS, "+"},
        {TokenType::MINUS, "-"},
        {TokenType::STAR, "*"},
        {TokenType::SLASH, "/"},
        {TokenType::PERCENT, "%"},
        {TokenType::AND, "and"},
        {TokenType::OR, "or"},
        {TokenType::LESS, "<"},
        {TokenType::LESS_EQUAL, "<="},
        {TokenType::GREATER, ">"},
        {TokenType::GREATER_EQUAL, ">="},
        {TokenType::EQUAL_EQUAL, "=="},
        {TokenType::NOT_EQUAL, "!="}
    };
    
    auto it = tokenTypeToString.find(type);
    if (it != tokenTypeToString.end()) {
        return it->second;
    }
    return "unknown";
}

Parser::Parser(const vector<Token>& tokens) : tokens(tokens) {}

json Parser::parse() {
    auto program = parseProgram();
    return program->toJson();
}

unique_ptr<Program> Parser::parseProgram() {
    auto program = make_unique<Program>();
    while (!isAtEnd()) {
        program->body.push_back(parseStatement());
    }
    return program;
}

unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::IF)) {
        return parseIfStatement();
    } else if (match(TokenType::WHILE)) {
        return parseWhileStatement();
    } else if (match(TokenType::IDENTIFIER)) {
        return parseAssignment();
    }
    throw runtime_error("Unexpected statement: " + peek().GetLexeme());
}

unique_ptr<ASTNode> Parser::parseIfStatement() {
    advance(); // Consume 'if'
    auto condition = parseExpression();

    if (!match(TokenType::THEN)) {
        throw runtime_error("Expected 'then' after condition.");
    }
    advance(); // Consume 'then'

    auto thenBranch = parseStatement();

    unique_ptr<ASTNode> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        advance(); // Consume 'else'
        elseBranch = parseStatement();
    }

    if (!match(TokenType::END)) {
        throw runtime_error("Expected 'end' after if statement.");
    }
    advance(); // Consume 'end'

    return make_unique<IfStatement>(move(condition), move(thenBranch), move(elseBranch));
}

unique_ptr<ASTNode> Parser::parseWhileStatement() {
    advance(); // Consume 'while'
    auto condition = parseExpression();

    if (!match(TokenType::DO)) {
        throw runtime_error("Expected 'do' after condition.");
    }
    advance(); // Consume 'do'

    auto body = parseStatement();

    if (!match(TokenType::END)) {
        throw runtime_error("Expected 'end' after while statement.");
    }
    advance(); // Consume 'end'

    return make_unique<WhileStatement>(move(condition), move(body));
}

unique_ptr<ASTNode> Parser::parseAssignment() {
    auto stmt = make_unique<AssignmentStatement>();

    if (peek().GetType() != TokenType::IDENTIFIER) {
        throw runtime_error("Expected variable name, found: " + peek().GetLexeme());
    }
    stmt->variable = advance().GetLexeme();

    if (advance().GetType() != TokenType::EQUAL) {
        throw runtime_error("Expected '=' after variable name, found: " + peek().GetLexeme());
    }

    stmt->expression = parseExpression();

    return stmt;
}

unique_ptr<ASTNode> Parser::parseExpression() {
    return parseLogicalExpr();
}

unique_ptr<ASTNode> Parser::parseLogicalExpr() {
    auto expr = parseComparisonExpr();

    while (match({TokenType::AND, TokenType::OR})) {
        string op = TokenTypeToString(advance().GetType());
        auto right = parseComparisonExpr();
        expr = make_unique<BinaryExpression>(op, move(expr), move(right));
    }

    return expr;
}

unique_ptr<ASTNode> Parser::parseComparisonExpr() {
    auto expr = parseArithmeticExpr();

    if (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
        string op = TokenTypeToString(advance().GetType());
        auto right = parseArithmeticExpr();
        expr = make_unique<BinaryExpression>(op, move(expr), move(right));
    }

    return expr;
}

unique_ptr<ASTNode> Parser::parseArithmeticExpr() {
    auto expr = parseTerm();

    while (match({TokenType::PLUS, TokenType::MINUS})) {
        string op = TokenTypeToString(advance().GetType());
        auto right = parseTerm();
        expr = make_unique<BinaryExpression>(op, move(expr), move(right));
    }

    return expr;
}

unique_ptr<ASTNode> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
        string op = TokenTypeToString(advance().GetType());
        auto right = parseFactor();
        expr = make_unique<BinaryExpression>(op, move(expr), move(right));
    }

    return expr;
}

unique_ptr<ASTNode> Parser::parseFactor() {
    if (match({TokenType::MINUS, TokenType::NOT})) {
        string op = TokenTypeToString(advance().GetType());
        auto right = parseFactor();
        return make_unique<UnaryExpression>(op, move(right));
    }

    if (match(TokenType::NUMBER)) {
        return make_unique<Literal>(advance().GetLexeme(), "Number");
    } else if (match(TokenType::STRING)) {
        return make_unique<Literal>(advance().GetLexeme(), "String");
    } else if (match({TokenType::NIL, TokenType::TRUE, TokenType::FALSE})) {
        return make_unique<Literal>(advance().GetLexeme(), "Boolean");
    } else if (match(TokenType::IDENTIFIER)) {
        return make_unique<Variable>(advance().GetLexeme());
    } else if (match(TokenType::LEFT_PAREN)) {
        advance();
        auto expr = parseExpression();
        if (advance().GetType() != TokenType::RIGHT_PAREN) {
            throw runtime_error("Expected ')' after expression");
        }
        return expr;
    }

    throw runtime_error("Unexpected factor: " + peek().GetLexeme());
}

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    return peek().GetType() == type;
}

bool Parser::match(vector<TokenType> types) {
    if (isAtEnd()) return false;
    for (auto type : types) {
        if (peek().GetType() == type) {
            return true;
        }
    }
    return false;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

Token Parser::peek() {
    if (isAtEnd()) return Token(TokenType::EOF_TOKEN, "");
    return tokens[current];
}

bool Parser::isAtEnd() {
    return current >= tokens.size() || tokens[current].GetType() == TokenType::EOF_TOKEN;
}