#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "../headers/lexer.h"
#include "../headers/parser.h"
#include "../headers/interpreter.h"
#include "../headers/repl.h"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

string ReadFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    string content, line;
    while (getline(file, line)) {
        content += line;
        content += "\n";
    }

    file.close();
    return content;
}

void WriteJsonToFile(const string& filename, const json& j) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    file << j.dump(2);  // Pretty-print the JSON with an indentation of 2 spaces
    file.close();
}

int main(int argc, char** argv) {
    string filename = "", input = "";

    if (argc == 2) {
        filename = argv[1];
        if (filename.substr(filename.find_last_of(".") + 1) != "sia") {
            cout << "Invalid file extension" << endl;
            return 1;
        }
        input = ReadFile(filename);
    } else if (argc > 2) {
        cout << "Usage: sia [filename]" << endl;
        return 1;
    } else if (argc < 2) {
        Repl::Start(cin, cout);
        return 1;
    }

    Lexer lexer = Lexer(input);
    vector<Token> tokens = lexer.scanTokens();
    tokens.push_back(Token(TokenType::EOF_TOKEN, ""));

    Parser parser(tokens);
    json ast = parser.parse();
    auto program = parser.parseProgram();

    string outputFilename = "ast.json";
    WriteJsonToFile(outputFilename, ast);
 
    // Printing the AST to a file
    // cout << "AST written to " << outputFilename << endl;

    // Printing the AST to the console
    cout << ast.dump(2) << endl;

    Interpreter interpreter;
    interpreter.interpret(program);

    return 0;
}