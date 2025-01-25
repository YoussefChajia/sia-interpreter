// #include <iostream>
// #include "printer.hpp"
//
//
// void Printer::print_indent(int indent) {
//     cout << string(indent * 2, ' ');
// }
//
// void Printer::print_comma(bool last_item) {
//     cout << (last_item ? "" : ",") << endl;
// }
//
// void Printer::print_ast(const unique_ptr<Node>& node, int indent) {
//     print_indent(indent);
//     cout << "{" << endl;
//     
//     print_indent(indent + 1);
//     cout << "\"type\": \"" << node->type << "\"";
//
//     if (auto program = dynamic_pointer_cast<Program>(node)) {
//         cout << "," << endl;
//         print_indent(indent + 1);
//         cout << "\"body\": [";
//
//         if (program->body.size() != 0) cout << endl;
//         
//         for (size_t i = 0; i < program->body.size(); ++i) {
//             print_ast(program->body[i], indent + 2);
//             print_comma(i == program->body.size() - 1);
//         }
//         
//         if (program->body.size() != 0) print_indent(indent + 1);
//         cout << "]";
//     }
//     else if (auto block_stmt = dynamic_pointer_cast<BlockStatement>(node)) {
//         cout << "," << endl;
//         print_indent(indent + 1);
//         cout << "\"body\": [";
//
//         if (block_stmt->body.size() != 0) cout << endl;
//         
//         for (size_t i = 0; i < block_stmt->body.size(); ++i) {
//             print_ast(block_stmt->body[i], indent + 2);
//             print_comma(i == block_stmt->body.size() - 1);
//         }
//         
//         if (block_stmt->body.size() != 0) print_indent(indent + 1);
//         cout << "]";
//     }
//     else if (auto expr_stmt = dynamic_pointer_cast<ExpressionStatement>(node)) {
//         cout << "," << endl;
//         print_indent(indent + 1);
//         cout << "\"expression\": " << endl;
//         print_ast(expr_stmt->expression, indent + 1);
//     }
//     else if (auto ass_expr = dynamic_pointer_cast<AssignmentExpression>(node)) {
//         cout << "," << endl;
//         print_indent(indent + 1);
//         cout << "\"identifier\": \"" << ass_expr->identifier << "\"";
//         
//         cout << "," << endl;
//         print_indent(indent + 1);
//         cout << "\"expression\": " << endl;
//         print_ast(ass_expr->expression, indent + 1);
//     }
//     else if (auto str_literal = dynamic_pointer_cast<StringLiteral>(node)) {
//         cout << "," << endl;
//         print_indent(indent + 1);
//         cout << "\"value\": \"" << str_literal->value << "\"";
//     }
//     else if (auto num_literal = dynamic_pointer_cast<NumericLiteral>(node)) {
//         cout << "," << endl;
//         print_indent(indent + 1);
//         cout << "\"value\": " << num_literal->value;
//     }
//     
//     cout << endl;
//     print_indent(indent);
//     cout << "}";
// }
