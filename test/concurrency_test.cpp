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
        throw; // Re-throw after logging failure
    }
}

// Fixture for Evaluator tests to avoid recreating Evaluator for each test
class ConcurrencyTest : public ::testing::Test {
protected:
    Parser parser;
    Evaluator evaluator;

    // Helper function to evaluate a string snippet
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
    
    // Helper to evaluate a variable's value after executing code
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

TEST_F(ConcurrencyTest, SimpleParallelExecution) {
    string code = R"(
        g_var1 = 0;
        g_var2 = "";
        parallel {
            { g_var1 = 10; }
            { g_var2 = "done"; }
        }
    )";
    ASSERT_NO_THROW(evaluate_code(code));
    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "g_var1")), 10);
    EXPECT_EQ(get_variant_value<string>(get_variable_value(code, "g_var2")), "done");
}

TEST_F(ConcurrencyTest, ParallelCounterIncrement) {
    string code = R"(
        counter = 0;
        parallel {
            { 
                i = 0;
                loop (5) { 
                    counter = counter + 1; 
                    i = i + 1; 
                    print("1. counter: ", counter, "| i : ", i);
                }
            }
            { 
                j = 0;
                loop (5) { 
                    counter = counter + 1; 
                    j = j + 1;
                    print("2. counter: ", counter, "| j : ", j);
                }
            }
        }
    )";

    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "counter")), 10);
    ASSERT_NO_THROW(evaluate_code(code));
}

TEST_F(ConcurrencyTest, ExceptionPropagation) {
    string code = R"(
        result = "not run";
        error_occurred = false;
        parallel {
            {
                result = "run";
                x = 10 / 0; // Should throw
                result = "error not propagated?";
            }
            {
                // This block should ideally complete or be cancelled depending on strategy
                // For now, just check if the error from the other block is thrown
            }
        }
        // This part should not be reached if exception is thrown correctly
        error_occurred = false; 
    )";

    ASSERT_THROW(evaluate_code(code), std::runtime_error);
}

TEST_F(ConcurrencyTest, ThreadLocalScopeIsolation) {
    string code = R"(
        global_val = 0;
        parallel {
            { 
                local_var = 1; 
                global_val = global_val + local_var; 
            }
            { 
                local_var = 2; // Should be a different local_var
                global_val = global_val + local_var; 
            }
        }
    )";

    EXPECT_EQ(get_variant_value<long>(get_variable_value(code, "global_val")), 3);
    EXPECT_THROW(get_variable_value(code, "local_var"), std::runtime_error);
    ASSERT_NO_THROW(evaluate_code(code));
}
