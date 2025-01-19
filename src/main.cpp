#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "parser.hpp"
#include "printer.hpp"

using namespace std;

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

void start_repl() {

    printf("Sia 0.1 - 2024\n");
    string line;
    Parser parser = Parser();
    Printer printer = Printer();
    
    while (true) {
        cout << ">> ";

        if (!getline(cin, line)) return;

        if (line == "clear") {
            system("clear");
            continue;
        }
        if (line == "quit") return;

        shared_ptr<Program> ast = parser.parse(line);
        printer.print_ast(ast);
        cout << endl;
    }
}

int main (int argc, char *argv[]) {

    string filename, input;

    if (argc == 2) {
        filename = argv[1];
        if (filename.substr(filename.find_last_of(".") + 1) != "sia") {
            cout << "Invalid file extension" << endl;
            return 1;
        }
        input = read_file(filename);
        Parser parser = Parser();
        Printer printer = Printer();
        try {
            shared_ptr<Program> ast = parser.parse(input);
            printer.print_ast(ast);
            cout << endl;
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    } else if (argc > 2) {
        cout << "Usage: sia [filename]" << endl;
        return 1;

    } else if (argc < 2) {
        start_repl();
        return 1;
    }

    return 0;
}
