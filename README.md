# Sia Interpreter

The Sia Interpreter is a simple interpreter for a new programming language named Sia. It includes a scanner (lexer), parser, and semantic analyzer to evaluate and execute Sia code. This project is intended for educational purposes, providing insights into the implementation of interpreters and the underlying concepts of programming languages.

## Description

The Sia Interpreter is designed to read input source code, convert it into tokens, parse the tokens into an Abstract Syntax Tree (AST), and then evaluate the AST to execute the code. The interpreter supports basic features such as variable assignments, arithmetic operations, boolean operations, conditional statements (if-else), and loops (while).

## Features

- Tokenizes source code into keywords, identifiers, operators, literals, and more.
- Parses tokens into an Abstract Syntax Tree (AST).
- Evaluates expressions and executes statements.
- Manages the runtime environment using a symbol table.
- Basic error handling for syntactical and semantic errors.

## Usage

### Prerequisites

- Ensure you have a C++ compiler installed.
- [Gradle](https://gradle.org/) is required for building the project.
- [nlohmann/json](https://github.com/nlohmann/json) library is required for JSON handling.

### Build and Run

1. Clone the repository:

   ```bash
   git clone https://github.com/YoussefChajia/sia-interpreter.git
   cd sia-interpreter
   ```

2. Build the project using Gradle:

   ```bash
   gradle build
   ```

3. Create an alias to simplify running the interpreter:

   ```bash
   alias sia='./app/build/exe/main/debug/app'
   ```

4. Run the interpreter with a Sia source file:

   ```bash
   sia path/to/source_file.sia
   ```

### Example

Create a Sia source file `example.sia` with the following content:

```sia
x = 10
if x > 5 then
    y = x * 2
else
    y = x / 2
end
```

Run the interpreter:

```bash
sia example.sia
```

### Note

To make the alias permanent, you can add the alias command to your shell configuration file (e.g., `.bashrc`, `.zshrc`).

## Contact

For any questions or inquiries, please contact Youssef Chajia at [Youssefchajia62@gmail.com].
