#include "../src/evaluator.hpp"
#include "../src/parser.hpp"
#include "../src/ast.hpp"
#include <gtest/gtest.h>
#include <variant>
#include <string>
#include <stdexcept>
#include <typeinfo>

using namespace std;

// Human-readable type name
template <typename T>
string get_type_name() {
    if (is_same<T, long>::value) return "long";
    if (is_same<T, double>::value) return "double";
    if (is_same<T, string>::value) return "string";
    if (is_same<T, bool>::value) return "bool";
    if (is_same<T, monostate>::value) return "monostate";
    return "unknown";
}

// Helper to get variant value easily or throw
template <typename T> 
T get_variant_value(const my_variant& v) {
    try {
        return get<T>(v);
    } catch (const bad_variant_access& e) {
        string type_info = "unknown";
        if (holds_alternative<long>(v)) type_info = "long";
        else if (holds_alternative<double>(v)) type_info = "double";
        else if (holds_alternative<string>(v)) type_info = "string";
        else if (holds_alternative<bool>(v)) type_info = "bool";
        else if (holds_alternative<monostate>(v)) type_info = "monostate";
        
        ADD_FAILURE() << "Unexpected variant type! Expected " << get_type_name<T>()
                     << " but variant contains " << type_info 
                     << ". Error: " << e.what();
        throw;
    }
}

// Fixture for Evaluator tests to avoid recreating Evaluator for each test
class EvaluatorTest : public ::testing::Test {
protected:
    Parser parser;
    Evaluator evaluator;

    my_variant evaluate_code(const string& code) {
        unique_ptr<ProgramNode> program = parser.parse(code);
        
        if (!program || program->statements.empty()) {
            throw std::runtime_error("Failed to parse expression or empty program in evaluate_code helper");
        }

        evaluator.evaluate(*program);
        
        if (program->statements.size() > 0) {
            auto* last_stmt = dynamic_cast<ExpressionStatementNode*>(program->statements.back().get());
            if (last_stmt && last_stmt->expression) {
                return evaluator.evaluate_expression(*last_stmt->expression);
            }
        }
        
        return monostate();
    }
    
    my_variant get_variable_value(const string& code, const string& variable_name) {
        unique_ptr<ProgramNode> program = parser.parse(code);
        evaluator.evaluate(*program);
        
        try {
            auto variable_node = make_unique<VariableNode>(variable_name, 0, 0);
            
            my_variant result = evaluator.evaluate_expression(*variable_node);
            
            return result;
        } catch (const exception& e) {
            cerr << "Exception getting variable '" << variable_name << "': " << e.what() << endl;
            throw;
        }
    }
};

TEST_F(EvaluatorTest, ArithmeticOperations) {
    string code = R"(
        a = 5 + 3;
        b = 10 - 4;
        c = 6 * 7;
        d = 10 / 4;
        e = 10 % 3;
        f = 2.5 * 2.0;
        g = 5.0 + 3;
    )";
    
    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "a")), 8);
    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "b")), 6);
    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "c")), 42);
    EXPECT_EQ(get_variant_value<double>(get_variable_value(code, "d")), 2.5);
    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "e")), 1);
    EXPECT_EQ(get_variant_value<double>(get_variable_value(code, "f")), 5.0);
    EXPECT_EQ(get_variant_value<double>(get_variable_value(code, "g")), 8);
    
    // Division by zero tests
    EXPECT_THROW(evaluate_code("error1 = 10 / 0;"), runtime_error);
    EXPECT_THROW(evaluate_code("error2 = 10.0 / 0.0;"), runtime_error);
    
    // Modulo requires integers
    EXPECT_THROW(evaluate_code("error3 = 10.5 % 3;"), runtime_error);
}

TEST_F(EvaluatorTest, ComparisonOperations) {
    string code = R"(
        a = 5 < 10;
        b = 5 > 10;
        c = 5 <= 5;
        d = 10 >= 10;
        e = 42 == 42;
        f = 42 != 42;
        g = "hello" == "hello";
        h = "hello" != "world";
        i = true == true;
        j = false != true;
    )";
    
    // Execute the code once to set up all variables
    evaluate_code(code);
    
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "a")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "b")), false);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "c")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "d")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "e")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "f")), false);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "g")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "h")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "i")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "j")), true);
}

TEST_F(EvaluatorTest, LogicalOperations) {
    string code = R"(
        a = true or false;
        b = false or false;
        c = true and true;
        d = true and false;
    )";
    
    evaluate_code(code);
    
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "a")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "b")), false);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "c")), true);
    EXPECT_EQ(get_variant_value<bool>(get_variable_value(code, "d")), false);
    EXPECT_THROW(evaluate_code("e = (1 > 0) or (1 / 0 == 1);"), runtime_error);
    EXPECT_THROW(evaluate_code("f = (1 < 0) and (1 / 0 == 1);"), runtime_error);
}

TEST_F(EvaluatorTest, VariableOperations) {
    string code = R"(
        x = 10;
        y = x + 5;
        x = "changed";
    )";
    
    evaluate_code(code);
    
    EXPECT_EQ(get_variant_value<string>(get_variable_value(code, "x")), "changed");
    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "y")), 15);

    EXPECT_THROW(get_variable_value(code, "z"), runtime_error);
}

TEST_F(EvaluatorTest, ScopeOperations) {
    string code = R"(
        a = 1;
        {
            a = 2;
            b = 3;
        }
    )";
    
    evaluate_code(code);
    
    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "a")), 2);
    
    EXPECT_THROW(get_variable_value(code, "b"), runtime_error);
}

TEST_F(EvaluatorTest, FunctionCallNative) {
    string code = R"(
        result = pow(2, 3);
        print("Hello", 123);  // Should print to stdout
    )";
    
    evaluate_code(code);
    
    EXPECT_EQ(get_variant_value<double>(get_variable_value(code, "result")), 8.0);
    
    EXPECT_NO_THROW(evaluate_code("print(\"Testing direct function call\");"));
    
    // Test errors
    EXPECT_THROW(evaluate_code("pow(2);"), runtime_error);
    EXPECT_THROW(evaluate_code("non_existent_func();"), runtime_error);
}
