#include "../src/evaluator.hpp"
#include "../src/parser.hpp"
#include <gtest/gtest.h>
#include <string>
#include <stdexcept>

using namespace std;

// Helper to get variant value easily or throw (from evaluator_test.cpp)
template <typename T> 
T get_variant_value(const my_variant& v) {
    try {
        return get<T>(v);
    } catch (const bad_variant_access& e) {
        ADD_FAILURE() << "Unexpected variant type. Expected " << typeid(T).name() << ". Error: " << e.what();
        throw; 
    }
}

// Fixture for Concurrency tests
class ConcurrencyTest : public ::testing::Test {
protected:
    Parser parser;
    Evaluator evaluator; // Each test gets a fresh evaluator and thread pool

    // Helper to evaluate a full program string
    void evaluate_program(const string& code) {
        unique_ptr<ProgramNode> program = parser.parse(code);
        ASSERT_NE(program, nullptr);
        evaluator.evaluate(*program);
    }

    // Helper to evaluate a single expression and get its value
    my_variant evaluate_expression(const string& expr_code) {
        string program_code = expr_code + ";"; 
        unique_ptr<ProgramNode> program = parser.parse(program_code);
        ASSERT_NE(program, nullptr);
        ASSERT_EQ(program->statements.size(), 1);
        auto* expr_stmt = dynamic_cast<ExpressionStatementNode*>(program->statements[0].get());
        ASSERT_NE(expr_stmt, nullptr);
        return evaluator.evaluate_expression(*expr_stmt->expression);
    }

    // Helper to get a variable's value after execution
    my_variant get_variable_value(const string& var_name) {
         // Need a way to access evaluator's internal state or evaluate the variable
         // For now, just evaluate the variable name as an expression
         return evaluate_expression(var_name);
    }
};

TEST_F(ConcurrencyTest, SimpleParallelExecution) {
    // Tests if two blocks run without crashing and modify global state correctly (assuming internal mutex works)
    string code = R"(
        g_var1 = 0;
        g_var2 = "";
        parallel {
            { g_var1 = 10; }
            { g_var2 = "done"; }
        }
    )";
    ASSERT_NO_THROW(evaluate_program(code));
    EXPECT_EQ(get_variant_value<long>(get_variable_value("g_var1")), 10);
    EXPECT_EQ(get_variant_value<string>(get_variable_value("g_var2")), "done");
}

TEST_F(ConcurrencyTest, ParallelCounterIncrement) {
    // Tests basic synchronization on shared global variable access
    // Relies on the internal mutex within evaluator's set_variable for global scope
    string code = R"(
        counter = 0;
        parallel {
            { 
                i = 0;
                loop (50) { 
                    counter = counter + 1; 
                    i = i + 1; 
                }
            }
            { 
                j = 0;
                loop (50) { 
                    counter = counter + 1; 
                    j = j + 1;
                }
            }
        }
    )";
    ASSERT_NO_THROW(evaluate_program(code));
    // Without proper locking inside evaluate_binary_op or explicit Sia locks, this might fail
    // But it tests the current set_variable locking for the final assignment
    EXPECT_EQ(get_variant_value<long>(get_variable_value("counter")), 100);
}

TEST_F(ConcurrencyTest, ExceptionPropagation) {
    // Tests if an exception in one parallel block is caught and propagated
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

    // We expect the runtime_error from division by zero to be thrown
    // by the evaluate_program call (specifically during future.get() in evaluate_parallel_block)
    ASSERT_THROW(evaluate_program(code), std::runtime_error);
    
    // Check if the variable before the error was set (optional, depends on exact execution order)
    // EXPECT_EQ(get_variant_value<string>(get_variable_value("result")), "run");
}

TEST_F(ConcurrencyTest, ThreadLocalScopeIsolation) {
    // Tests if variables defined within parallel blocks don't interfere
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
    ASSERT_NO_THROW(evaluate_program(code));
    // Each block adds its local_var (1 and 2) to global_val (initially 0)
    EXPECT_EQ(get_variant_value<long>(get_variable_value("global_val")), 3);
    // Attempting to access local_var outside should fail
    EXPECT_THROW(get_variable_value("local_var"), std::runtime_error);
}

// TODO: Add tests for nested parallel blocks if supported
// TODO: Add stress tests with more blocks/iterations
// TODO: Add tests using Thread Sanitizer for race detection

} 