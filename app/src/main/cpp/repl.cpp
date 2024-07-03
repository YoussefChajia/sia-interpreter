#include <iostream>
#include <string>
#include "../headers/lexer.h"
#include "../headers/repl.h"

using namespace std;

const string PROMPT = ">> ";

void Repl::Start(istream& in, ostream& out) {
    string line;
    
    while (true) {
        out << PROMPT;

        if (!getline(in, line)) return;

        // type "quit" to exit the REPL
        if (line == "quit") return;

        Lexer lexer = Lexer(line);
        vector<Token> tokens = lexer.scanTokens();

        for (Token token : tokens) {
            cout << "Token : " << token.GetLexeme() << endl;
        }
    }
}
