#include "../src/parser.hpp"
#include "../src/ast.hpp"
#include "../src/token.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace std;

// Helper function to check if a node is of a specific type
template <typename T>
bool is_node_type(const ASTNode* node) {
    return dynamic_cast<const T*>(node) != nullptr;
}

TEST(ParserTest, SimpleAssignment) {
    Parser parser;
    string input = "x = 42;";
    unique_ptr<ProgramNode> program = parser.parse(input);

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    StatementNode* stmt = program->statements[0].get();
    ASSERT_TRUE(is_node_type<AssignmentNode>(stmt));

    AssignmentNode* assignment = static_cast<AssignmentNode*>(stmt);
    EXPECT_EQ(assignment->identifier, "x");
    ASSERT_TRUE(is_node_type<LongNumberLiteral>(assignment->expression.get()));

    LongNumberLiteral* literal = static_cast<LongNumberLiteral*>(assignment->expression.get());
    EXPECT_EQ(literal->value, 42);
}

TEST(ParserTest, ArithmeticExpression) {
    Parser parser;
    string input = "y = (5 + 3) * 2;";
    unique_ptr<ProgramNode> program = parser.parse(input);

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    StatementNode* stmt = program->statements[0].get();
    ASSERT_TRUE(is_node_type<AssignmentNode>(stmt));

    AssignmentNode* assignment = static_cast<AssignmentNode*>(stmt);
    EXPECT_EQ(assignment->identifier, "y");
    
    // Check top-level operation (*)
    ASSERT_TRUE(is_node_type<BinaryOpNode>(assignment->expression.get()));
    BinaryOpNode* multiply = static_cast<BinaryOpNode*>(assignment->expression.get());
    EXPECT_EQ(multiply->op, TokenType::MULTIPLY);
    ASSERT_TRUE(is_node_type<LongNumberLiteral>(multiply->right.get()));
    EXPECT_EQ(static_cast<LongNumberLiteral*>(multiply->right.get())->value, 2);

    // Check left operand of multiply (+)
    ASSERT_TRUE(is_node_type<BinaryOpNode>(multiply->left.get()));
    BinaryOpNode* add = static_cast<BinaryOpNode*>(multiply->left.get());
    EXPECT_EQ(add->op, TokenType::PLUS);
    ASSERT_TRUE(is_node_type<LongNumberLiteral>(add->left.get()));
    EXPECT_EQ(static_cast<LongNumberLiteral*>(add->left.get())->value, 5);
    ASSERT_TRUE(is_node_type<LongNumberLiteral>(add->right.get()));
    EXPECT_EQ(static_cast<LongNumberLiteral*>(add->right.get())->value, 3);
}

TEST(ParserTest, FunctionDefinition) {
    Parser parser;
    string input = "function myFunc(a, b) { return a + b; }";
    unique_ptr<ProgramNode> program = parser.parse(input);

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    StatementNode* stmt = program->statements[0].get();
    ASSERT_TRUE(is_node_type<FunctionDefNode>(stmt));

    FunctionDefNode* funcDef = static_cast<FunctionDefNode*>(stmt);
    EXPECT_EQ(funcDef->name, "myFunc");
    ASSERT_EQ(funcDef->parameters.size(), 2);
    EXPECT_EQ(funcDef->parameters[0], "a");
    EXPECT_EQ(funcDef->parameters[1], "b");

    ASSERT_NE(funcDef->body, nullptr);
    ASSERT_EQ(funcDef->body->statements.size(), 1);
    ASSERT_TRUE(is_node_type<ReturnNode>(funcDef->body->statements[0].get()));
}

TEST(ParserTest, ParallelBlock) {
    Parser parser;
    string input = "parallel { x = 1; } { y = 2; }";
    unique_ptr<ProgramNode> program = parser.parse(input);

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    StatementNode* stmt = program->statements[0].get();
    ASSERT_TRUE(is_node_type<ParallelBlockNode>(stmt));

    ParallelBlockNode* parallelBlock = static_cast<ParallelBlockNode*>(stmt);
    ASSERT_EQ(parallelBlock->blocks.size(), 2);
    
    // Check first block
    ASSERT_NE(parallelBlock->blocks[0], nullptr);
    ASSERT_EQ(parallelBlock->blocks[0]->statements.size(), 1);
    ASSERT_TRUE(is_node_type<AssignmentNode>(parallelBlock->blocks[0]->statements[0].get()));

    // Check second block
    ASSERT_NE(parallelBlock->blocks[1], nullptr);
    ASSERT_EQ(parallelBlock->blocks[1]->statements.size(), 1);
    ASSERT_TRUE(is_node_type<AssignmentNode>(parallelBlock->blocks[1]->statements[0].get()));
}

TEST(ParserTest, SyntaxError) {
    Parser parser;
    string input = "x = 5 +;"; // Missing operand after +
    ASSERT_THROW(parser.parse(input), std::runtime_error);
    // try {
    //     parser.parse(input);
    //     FAIL() << "Expected std::runtime_error for syntax error";
    // } catch (const std::runtime_error& e) {
    //     // Check if the error message indicates an unexpected token (e.g., semicolon)
    //     EXPECT_NE(string(e.what()).find("Unexpected token"), string::npos);
    //     EXPECT_NE(string(e.what()).find("SEMICOLON"), string::npos);
    // } catch (...) {
    //     FAIL() << "Expected std::runtime_error, but got different exception";
    // }
} 