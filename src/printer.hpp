#include "parser.hpp"

class Printer {
public:
    Printer() = default;
    void print_indent(int indent);
    void print_comma(bool last_item);
    void print_ast(const shared_ptr<Node>& node, int indent = 0);
    virtual ~Printer() = default;
};
