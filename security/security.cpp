#include "security.h"
#include <iostream>

using namespace std;

void SecurityAnalyzer::analyze(shared_ptr<ProgramNode> program) {
    for (auto& stmt : program->statements) {
        analyzeStatement(stmt);
    }
}

void SecurityAnalyzer::printReport() {
    if (issues.empty()) {
        cout << "Security analysis passed. No issues found.\n";
        return;
    }

    cout << "\n===== Security Analysis Report =====\n";
    for (auto& issue : issues) {
        cout << "[" << issue.severity << "] " << issue.message << "\n";
    }
    cout << "====================================\n";
    cout << "Total issues found: " << issues.size() << "\n";
}

void SecurityAnalyzer::analyzeStatement(shared_ptr<ASTNode> node) {
    if (auto n = dynamic_pointer_cast<VarDeclNode>(node)) {
        analyzeVarDecl(n);
    }
    else if (auto n = dynamic_pointer_cast<AssignNode>(node)) {
        analyzeAssign(n);
    }
    else if (auto n = dynamic_pointer_cast<IfNode>(node)) {
        analyzeIf(n);
    }
    else if (auto n = dynamic_pointer_cast<WhileNode>(node)) {
        analyzeWhile(n);
    }
    else if (auto n = dynamic_pointer_cast<ReturnNode>(node)) {
        analyzeReturn(n);
    }
}

void SecurityAnalyzer::analyzeVarDecl(shared_ptr<VarDeclNode> node) {
    if (node->initializer) {
        analyzeExpression(node->initializer);
        initializedVars[node->name] = true;
    }
    else {
        // declared but no value given, mark it as uninitialized
        initializedVars[node->name] = false;
    }
    varTypes[node->name] = node->type;
}

void SecurityAnalyzer::analyzeAssign(shared_ptr<AssignNode> node) {
    // check if variable was ever declared
    if (initializedVars.find(node->varName) == initializedVars.end()) {
        issues.push_back({"ERROR", "variable '" + node->varName + "' is assigned but was never declared"});
    }

    analyzeExpression(node->value);
    initializedVars[node->varName] = true;
}

void SecurityAnalyzer::analyzeIf(shared_ptr<IfNode> node) {
    analyzeExpression(node->condition);

    for (auto& stmt : node->body) {
        analyzeStatement(stmt);
    }
    for (auto& stmt : node->elseBody) {
        analyzeStatement(stmt);
    }
}

void SecurityAnalyzer::analyzeWhile(shared_ptr<WhileNode> node) {
    // check if the loop condition is always true which means infinite loop
    if (isAlwaysTrue(node->condition)) {
        issues.push_back({"WARNING", "infinite loop detected, while condition is always true"});
    }

    analyzeExpression(node->condition);

    for (auto& stmt : node->body) {
        analyzeStatement(stmt);
    }
}

void SecurityAnalyzer::analyzeReturn(shared_ptr<ReturnNode> node) {
    analyzeExpression(node->value);
}

void SecurityAnalyzer::analyzeExpression(shared_ptr<ASTNode> node) {

    if (auto n = dynamic_pointer_cast<IdentifierNode>(node)) {
        // variable used but never initialized
        if (initializedVars.find(n->name) != initializedVars.end()) {
            if (!initializedVars[n->name]) {
                issues.push_back({"ERROR", "variable '" + n->name + "' is used before being initialized"});
            }
        }
    }

    if (auto n = dynamic_pointer_cast<BinaryOpNode>(node)) {
        analyzeExpression(n->left);
        analyzeExpression(n->right);

        // check for division by zero
        if (n->op == "/") {
            checkDivisionByZero(n);
        }
    }
}

void SecurityAnalyzer::checkDivisionByZero(shared_ptr<BinaryOpNode> node) {
    if (auto n = dynamic_pointer_cast<NumberNode>(node->right)) {
        // if the right side is literally 0 or 0.0
        if (n->value == "0" || n->value == "0.0") {
            issues.push_back({"ERROR", "division by zero detected"});
        }
    }
}

// checks if a condition like (1 > 0) or (5 == 5) is always true
bool SecurityAnalyzer::isAlwaysTrue(shared_ptr<ASTNode> node) {
    if (auto n = dynamic_pointer_cast<BinaryOpNode>(node)) {
        auto left  = dynamic_pointer_cast<NumberNode>(n->left);
        auto right = dynamic_pointer_cast<NumberNode>(n->right);

        if (left && right) {
            double l = stod(left->value);
            double r = stod(right->value);

            if (n->op == ">")  return l > r;
            if (n->op == "<")  return l < r;
            if (n->op == "==") return l == r;
            if (n->op == "!=") return l != r;
            if (n->op == ">=") return l >= r;
            if (n->op == "<=") return l <= r;
        }
    }
    return false;
}