#include "../headers/ASTNode.h"

json Program::toJson() const {
    json j;
    j["type"] = "Program";
    j["body"] = json::array();
    for (const auto& stmt : body) {
        j["body"].push_back(stmt->toJson());
    }
    return j;
}

json AssignmentStatement::toJson() const {
    return json{
        {"type", "AssignmentStatement"},
        {"variable", {
            {"type", "Identifier"},
            {"name", variable}
        }},
        {"value", expression->toJson()}
    };
}

json BinaryExpression::toJson() const {
    return json{
        {"type", "BinaryExpression"},
        {"operator", op},
        {"left", left->toJson()},
        {"right", right->toJson()}
    };
}

json UnaryExpression::toJson() const {
    return json{
        {"type", "UnaryExpression"},
        {"operator", op},
        {"operand", operand->toJson()}
    };
}

json Literal::toJson() const {
    return json{
        {"type", type + "Literal"},
        {"value", value}
    };
}

json Variable::toJson() const {
    return json{
        {"type", "Identifier"},
        {"name", name}
    };
}

json IfStatement::toJson() const {
    json j;
    j["type"] = "IfStatement";
    j["condition"] = condition->toJson();
    j["thenBranch"] = thenBranch->toJson();
    if (elseBranch) {
        j["elseBranch"] = elseBranch->toJson();
    } else {
        j["elseBranch"] = nullptr;
    }
    return j;
}

json WhileStatement::toJson() const {
    return json{
        {"type", "WhileStatement"},
        {"condition", condition->toJson()},
        {"body", body->toJson()}
    };
}