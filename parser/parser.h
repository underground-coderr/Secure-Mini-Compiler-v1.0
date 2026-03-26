#pragma once
#include <vector>
#include <memory>
#include "../lexer/lexer.h"
#include "../ast/ast.h"

using namespace std;

class Parser {
public:
    Parser(const vector<Token>& tokens);
    shared_ptr<ProgramNode> parse();

private:
    vector<Token> tokens;
    size_t pos;   // changed from int to size_t

    Token current();
    Token peek(int offset = 1);
    Token consume();
    Token expect(TokenType type, const string& errorMsg);
    bool check(TokenType type);

    shared_ptr<ASTNode> parseStatement();
    shared_ptr<ASTNode> parseVarDecl(const string& type);
    shared_ptr<ASTNode> parseAssignment();
    shared_ptr<ASTNode> parseIf();
    shared_ptr<ASTNode> parseWhile();
    shared_ptr<ASTNode> parseReturn();
    shared_ptr<ASTNode> parseExpression();
    shared_ptr<ASTNode> parseComparison();
    shared_ptr<ASTNode> parseAddSub();
    shared_ptr<ASTNode> parseMulDiv();
    shared_ptr<ASTNode> parsePrimary();

    vector<shared_ptr<ASTNode>> parseBlock();
};