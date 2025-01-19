#include <iostream>
#include <fstream>
#include "../src/parser.hpp"

using namespace std;

#include <fstream>
#include <sstream>
#include <stdexcept>

string read_file(const string& filepath);

int main() {
    string input = read_file("/home/dazorika/dev/cpp-dev/sia-parser/bin/main.sia");
    Parser parser = Parser(input);
    try {
        shared_ptr<Program> ast = parser.parse();
        parser.print_ast(ast);
        cout << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    return 0;
}

string read_file(const string& filepath) {
    if (filepath.substr(filepath.find_last_of(".") + 1) != "sia") {
        throw runtime_error("File must have .sia extension");
    }

    ifstream file(filepath);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filepath);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}
