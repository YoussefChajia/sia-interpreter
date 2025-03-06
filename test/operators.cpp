#include "evaluator.hpp"
#include "parser.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace std;

class SiaTest : public ::testing::Test {
protected:
    Parser parser;
    Evaluator evaluator;
    stringstream buffer;
    streambuf* old_cout;

    vector<string> test_files = {"../bin/everything.sia"};

    void SetUp() override {
        old_cout = cout.rdbuf(buffer.rdbuf());
    }

    void TearDown() override {
        cout.rdbuf(old_cout);
        buffer.str("");
    }

    string evaluate_code(const string& code) {
        auto program = parser.parse(code);
        evaluator.evaluate(*program);
        string output = buffer.str();
        buffer.str("");
        return output;
    }
};

TEST_F(SiaTest, AllTest) {

    string file_name = test_files[0];
    ifstream file(file_name);
    ASSERT_TRUE(file.is_open()) << "Failed to open " + file_name;

    stringstream codeStream;
    codeStream << file.rdbuf();
    const string code = codeStream.str();
    ASSERT_NO_THROW({
        string result = evaluate_code(code);
        EXPECT_EQ(result, "Number: 42\nString: Youssef\nBoolean: true\nGreeting: Hello, Youssef\nArithmetic result: 20\nNegative value: -20\nComplex math: 5\nSum: 7\nLoop counter: 3\nFinal result: 21\n");
    });
}

