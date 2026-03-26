#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../ast/ast.h"

using namespace std;

// holds info about a variable we have seen so far
struct VarInfo {
    string type;
    bool initialized;
};

class SemanticAnalyzer {
public:
    void analyze(shared_ptr<ProgramNode> program);

private:
    // scope stack, each map is one scope level
    vector<map<string, VarInfo>> scopeStack;

    void enterScope();
    void exitScope();

    void declareVar(const string& name, const string& type, bool initialized, int line);
    VarInfo& lookupVar(const string& name, int line);
    bool varExistsInCurrentScope(const string& name);

    void analyzeStatement(shared_ptr<ASTNode> node);
    void analyzeVarDecl(shared_ptr<VarDeclNode> node);
    void analyzeAssign(shared_ptr<AssignNode> node);
    void analyzeIf(shared_ptr<IfNode> node);
    void analyzeWhile(shared_ptr<WhileNode> node);
    void analyzeReturn(shared_ptr<ReturnNode> node);
    string analyzeExpression(shared_ptr<ASTNode> node);
};