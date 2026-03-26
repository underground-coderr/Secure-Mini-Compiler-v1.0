#pragma once
#include <string>
#include <vector>
#include <memory>

using namespace std;

// base class for every node in the tree
struct ASTNode {
    virtual ~ASTNode() {}
};

// a whole program is just a list of statements
struct ProgramNode : ASTNode {
    vector<shared_ptr<ASTNode>> statements;
};

// int x; or float y;
struct VarDeclNode : ASTNode {
    string type;
    string name;
    shared_ptr<ASTNode> initializer; // can be null if no value assigned
};

// x = expression;
struct AssignNode : ASTNode {
    string varName;
    shared_ptr<ASTNode> value;
};

// if (condition) { body } else { elseBody }
struct IfNode : ASTNode {
    shared_ptr<ASTNode> condition;
    vector<shared_ptr<ASTNode>> body;
    vector<shared_ptr<ASTNode>> elseBody;
};

// while (condition) { body }
struct WhileNode : ASTNode {
    shared_ptr<ASTNode> condition;
    vector<shared_ptr<ASTNode>> body;
};

// return expression;
struct ReturnNode : ASTNode {
    shared_ptr<ASTNode> value;
};

// any binary operation like x + 1 or x > 0
struct BinaryOpNode : ASTNode {
    string op;
    shared_ptr<ASTNode> left;
    shared_ptr<ASTNode> right;
};

// a number like 10 or 3.14
struct NumberNode : ASTNode {
    string value;
};

// a variable being used like x
struct IdentifierNode : ASTNode {
    string name;
};