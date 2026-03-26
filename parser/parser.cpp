#include "parser.h"
#include <stdexcept>

using namespace std;

Parser::Parser(const vector<Token>& tokens) {
    this->tokens = tokens;
    pos = 0;
}

Token Parser::current() {
    return tokens[pos];
}

Token Parser::peek(int offset) {
    size_t p = pos + offset;   // changed to size_t
    if (p < tokens.size())
        return tokens[p];
    return tokens[tokens.size() - 1];
}

Token Parser::consume() {
    Token t = tokens[pos];
    pos++;
    return t;
}

Token Parser::expect(TokenType type, const string& errorMsg) {
    if (current().type != type) {
        throw runtime_error("Line " + to_string(current().line) + ": " + errorMsg);
    }
    return consume();
}

bool Parser::check(TokenType type) {
    return current().type == type;
}

shared_ptr<ProgramNode> Parser::parse() {
    shared_ptr<ProgramNode> program = make_shared<ProgramNode>();

    while (!check(TokenType::END_OF_FILE)) {
        program->statements.push_back(parseStatement());
    }

    return program;
}

shared_ptr<ASTNode> Parser::parseStatement() {
    if (check(TokenType::KW_INT) || check(TokenType::KW_FLOAT)) {
        string type = consume().value;
        return parseVarDecl(type);
    }
    if (check(TokenType::KW_IF))     return parseIf();
    if (check(TokenType::KW_WHILE))  return parseWhile();
    if (check(TokenType::KW_RETURN)) return parseReturn();
    if (check(TokenType::IDENTIFIER)) return parseAssignment();

    throw runtime_error("Line " + to_string(current().line) + ": unexpected token '" + current().value + "'");
}

shared_ptr<ASTNode> Parser::parseVarDecl(const string& type) {
    shared_ptr<VarDeclNode> node = make_shared<VarDeclNode>();
    node->type = type;
    node->name = expect(TokenType::IDENTIFIER, "expected variable name").value;

    if (check(TokenType::OP_ASSIGN)) {
        consume();
        node->initializer = parseExpression();
    }

    expect(TokenType::SEMICOLON, "expected ';' after declaration");
    return node;
}

shared_ptr<ASTNode> Parser::parseAssignment() {
    shared_ptr<AssignNode> node = make_shared<AssignNode>();
    node->varName = consume().value;
    expect(TokenType::OP_ASSIGN, "expected '=' in assignment");
    node->value = parseExpression();
    expect(TokenType::SEMICOLON, "expected ';' after assignment");
    return node;
}

shared_ptr<ASTNode> Parser::parseIf() {
    shared_ptr<IfNode> node = make_shared<IfNode>();
    consume();
    expect(TokenType::LPAREN, "expected '(' after if");
    node->condition = parseExpression();
    expect(TokenType::RPAREN, "expected ')' after if condition");
    node->body = parseBlock();

    if (check(TokenType::KW_ELSE)) {
        consume();
        node->elseBody = parseBlock();
    }

    return node;
}

shared_ptr<ASTNode> Parser::parseWhile() {
    shared_ptr<WhileNode> node = make_shared<WhileNode>();
    consume();
    expect(TokenType::LPAREN, "expected '(' after while");
    node->condition = parseExpression();
    expect(TokenType::RPAREN, "expected ')' after while condition");
    node->body = parseBlock();
    return node;
}

shared_ptr<ASTNode> Parser::parseReturn() {
    shared_ptr<ReturnNode> node = make_shared<ReturnNode>();
    consume();
    node->value = parseExpression();
    expect(TokenType::SEMICOLON, "expected ';' after return");
    return node;
}

vector<shared_ptr<ASTNode>> Parser::parseBlock() {
    vector<shared_ptr<ASTNode>> statements;
    expect(TokenType::LBRACE, "expected '{'");

    while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE)) {
        statements.push_back(parseStatement());
    }

    expect(TokenType::RBRACE, "expected '}'");
    return statements;
}

shared_ptr<ASTNode> Parser::parseExpression() {
    return parseComparison();
}

shared_ptr<ASTNode> Parser::parseComparison() {
    shared_ptr<ASTNode> left = parseAddSub();

    while (check(TokenType::OP_EQ)  || check(TokenType::OP_NEQ) ||
           check(TokenType::OP_LT)  || check(TokenType::OP_GT)  ||
           check(TokenType::OP_LTE) || check(TokenType::OP_GTE)) {

        string op = consume().value;
        shared_ptr<ASTNode> right = parseAddSub();
        shared_ptr<BinaryOpNode> binop = make_shared<BinaryOpNode>();
        binop->op = op;
        binop->left = left;
        binop->right = right;
        left = binop;
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseAddSub() {
    shared_ptr<ASTNode> left = parseMulDiv();

    while (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
        string op = consume().value;
        shared_ptr<ASTNode> right = parseMulDiv();
        shared_ptr<BinaryOpNode> binop = make_shared<BinaryOpNode>();
        binop->op = op;
        binop->left = left;
        binop->right = right;
        left = binop;
    }

    return left;
}

shared_ptr<ASTNode> Parser::parseMulDiv() {
    shared_ptr<ASTNode> left = parsePrimary();

    while (check(TokenType::OP_MUL) || check(TokenType::OP_DIV)) {
        string op = consume().value;
        shared_ptr<ASTNode> right = parsePrimary();
        shared_ptr<BinaryOpNode> binop = make_shared<BinaryOpNode>();
        binop->op = op;
        binop->left = left;
        binop->right = right;
        left = binop;
    }

    return left;
}

shared_ptr<ASTNode> Parser::parsePrimary() {
    if (check(TokenType::INTEGER_LITERAL) || check(TokenType::FLOAT_LITERAL)) {
        shared_ptr<NumberNode> node = make_shared<NumberNode>();
        node->value = consume().value;
        return node;
    }
    if (check(TokenType::IDENTIFIER)) {
        shared_ptr<IdentifierNode> node = make_shared<IdentifierNode>();
        node->name = consume().value;
        return node;
    }
    if (check(TokenType::LPAREN)) {
        consume();
        shared_ptr<ASTNode> expr = parseExpression();
        expect(TokenType::RPAREN, "expected ')' to close expression");
        return expr;
    }

    throw runtime_error("Line " + to_string(current().line) + ": unexpected token '" + current().value + "'");
}