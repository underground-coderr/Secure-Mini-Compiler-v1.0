#include "analyzer.h"
#include <stdexcept>
#include <iostream>

using namespace std;

void SemanticAnalyzer::analyze(shared_ptr<ProgramNode> program) {
    enterScope();

    for (auto& stmt : program->statements) {
        analyzeStatement(stmt);
    }

    exitScope();
    // removed the cout line here, GUI handles output now
}

void SemanticAnalyzer::enterScope() {
    scopeStack.push_back(map<string, VarInfo>());
}

void SemanticAnalyzer::exitScope() {
    scopeStack.pop_back();
}

void SemanticAnalyzer::declareVar(const string& name, const string& type, bool initialized, int line) {
    if (varExistsInCurrentScope(name)) {
        throw runtime_error("Line " + to_string(line) + ": variable '" + name + "' is already declared in this scope");
    }
    scopeStack.back()[name] = {type, initialized};
}

VarInfo& SemanticAnalyzer::lookupVar(const string& name, int line) {
    for (int i = scopeStack.size() - 1; i >= 0; i--) {
        if (scopeStack[i].count(name)) {
            return scopeStack[i][name];
        }
    }
    throw runtime_error("Line " + to_string(line) + ": variable '" + name + "' was not declared");
}

bool SemanticAnalyzer::varExistsInCurrentScope(const string& name) {
    return scopeStack.back().count(name) > 0;
}

void SemanticAnalyzer::analyzeStatement(shared_ptr<ASTNode> node) {
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

void SemanticAnalyzer::analyzeVarDecl(shared_ptr<VarDeclNode> node) {
    bool initialized = false;

    if (node->initializer) {
        string exprType = analyzeExpression(node->initializer);

        if (node->type == "int" && exprType == "float") {
            cout << "Warning: assigning float value to int variable '" << node->name << "', precision will be lost.\n";
        }

        initialized = true;
    }

    declareVar(node->name, node->type, initialized, 0);
}

void SemanticAnalyzer::analyzeAssign(shared_ptr<AssignNode> node) {
    VarInfo& info = lookupVar(node->varName, 0);
    analyzeExpression(node->value);
    info.initialized = true;
}

void SemanticAnalyzer::analyzeIf(shared_ptr<IfNode> node) {
    analyzeExpression(node->condition);

    enterScope();
    for (auto& stmt : node->body) {
        analyzeStatement(stmt);
    }
    exitScope();

    if (!node->elseBody.empty()) {
        enterScope();
        for (auto& stmt : node->elseBody) {
            analyzeStatement(stmt);
        }
        exitScope();
    }
}

void SemanticAnalyzer::analyzeWhile(shared_ptr<WhileNode> node) {
    analyzeExpression(node->condition);

    enterScope();
    for (auto& stmt : node->body) {
        analyzeStatement(stmt);
    }
    exitScope();
}

void SemanticAnalyzer::analyzeReturn(shared_ptr<ReturnNode> node) {
    analyzeExpression(node->value);
}

string SemanticAnalyzer::analyzeExpression(shared_ptr<ASTNode> node) {
    if (auto n = dynamic_pointer_cast<NumberNode>(node)) {
        if (n->value.find('.') != string::npos)
            return "float";
        return "int";
    }

    if (auto n = dynamic_pointer_cast<IdentifierNode>(node)) {
        VarInfo& info = lookupVar(n->name, 0);

        if (!info.initialized) {
            cout << "Warning: variable '" << n->name << "' is used before being initialized.\n";
        }

        return info.type;
    }

    if (auto n = dynamic_pointer_cast<BinaryOpNode>(node)) {
        string leftType  = analyzeExpression(n->left);
        string rightType = analyzeExpression(n->right);

        if (leftType == "float" || rightType == "float")
            return "float";
        return "int";
    }

    return "unknown";
}