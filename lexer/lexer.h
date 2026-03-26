#pragma once
#include <string>
#include <vector>

using namespace std;

enum class TokenType {
    INTEGER_LITERAL, FLOAT_LITERAL, IDENTIFIER,
    KW_INT, KW_FLOAT, KW_IF, KW_ELSE, KW_WHILE, KW_RETURN,
    OP_PLUS, OP_MINUS, OP_MUL, OP_DIV,
    OP_ASSIGN,
    OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE,
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
    END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    string value;
    int line;

    Token(TokenType t, string v, int l) {
        type = t;
        value = v;
        line = l;
    }
};

class Lexer {
public:
    Lexer(const string& source);
    vector<Token> tokenize();

private:
    string source;
    size_t pos;   // changed from int to size_t
    int line;

    char current();
    char peek(int offset = 1);
    void advance();
    void skipWhitespaceAndComments();

    Token readNumber();
    Token readIdentifierOrKeyword();
    Token readOperatorOrDelimiter();

    TokenType keywordLookup(const string& word);
};