#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../ast/ast.h"

using namespace std;

struct SecurityIssue {
    string severity;
    string message;
};

class SecurityAnalyzer {
public:
    void analyze(shared_ptr<ProgramNode> program);
    void printReport();
    vector<SecurityIssue> getIssues() { return issues; }

private:
    vector<SecurityIssue> issues;

    map<string, bool> initializedVars;
    map<string, string> varTypes;

    void analyzeStatement(shared_ptr<ASTNode> node);
    void analyzeVarDecl(shared_ptr<VarDeclNode> node);
    void analyzeAssign(shared_ptr<AssignNode> node);
    void analyzeIf(shared_ptr<IfNode> node);
    void analyzeWhile(shared_ptr<WhileNode> node);
    void analyzeReturn(shared_ptr<ReturnNode> node);
    void analyzeExpression(shared_ptr<ASTNode> node);
    void checkDivisionByZero(shared_ptr<BinaryOpNode> node);
    bool isAlwaysTrue(shared_ptr<ASTNode> node);
};