#include "lexer.h"
#include <cctype>

using namespace std;

Lexer::Lexer(const string& source) {
    this->source = source;
    pos = 0;
    line = 1;
}

char Lexer::current() {
    if (pos < source.size())
        return source[pos];
    return '\0';
}

char Lexer::peek(int offset) {
    size_t p = pos + offset;   // changed to size_t
    if (p < source.size())
        return source[p];
    return '\0';
}

void Lexer::advance() {
    if (pos < source.size()) {
        if (source[pos] == '\n')
            line++;
        pos++;
    }
}

void Lexer::skipWhitespaceAndComments() {
    while (pos < source.size()) {
        if (isspace(current())) {
            advance();
        }
        else if (current() == '/' && peek() == '/') {
            while (current() != '\n' && current() != '\0')
                advance();
        }
        else {
            break;
        }
    }
}

Token Lexer::readNumber() {
    string num = "";
    bool isFloat = false;

    while (isdigit(current())) {
        num += current();
        advance();
    }

    if (current() == '.' && isdigit(peek())) {
        isFloat = true;
        num += current();
        advance();
        while (isdigit(current())) {
            num += current();
            advance();
        }
    }

    if (isFloat)
        return Token(TokenType::FLOAT_LITERAL, num, line);
    else
        return Token(TokenType::INTEGER_LITERAL, num, line);
}

Token Lexer::readIdentifierOrKeyword() {
    string word = "";

    while (isalnum(current()) || current() == '_') {
        word += current();
        advance();
    }

    TokenType type = keywordLookup(word);
    return Token(type, word, line);
}

TokenType Lexer::keywordLookup(const string& word) {
    if (word == "int")    return TokenType::KW_INT;
    if (word == "float")  return TokenType::KW_FLOAT;
    if (word == "if")     return TokenType::KW_IF;
    if (word == "else")   return TokenType::KW_ELSE;
    if (word == "while")  return TokenType::KW_WHILE;
    if (word == "return") return TokenType::KW_RETURN;
    return TokenType::IDENTIFIER;
}

Token Lexer::readOperatorOrDelimiter() {
    char c = current();
    advance();

    if (c == '=' && current() == '=') { advance(); return Token(TokenType::OP_EQ,  "==", line); }
    if (c == '!' && current() == '=') { advance(); return Token(TokenType::OP_NEQ, "!=", line); }
    if (c == '<' && current() == '=') { advance(); return Token(TokenType::OP_LTE, "<=", line); }
    if (c == '>' && current() == '=') { advance(); return Token(TokenType::OP_GTE, ">=", line); }

    if (c == '+') return Token(TokenType::OP_PLUS,   "+", line);
    if (c == '-') return Token(TokenType::OP_MINUS,  "-", line);
    if (c == '*') return Token(TokenType::OP_MUL,    "*", line);
    if (c == '/') return Token(TokenType::OP_DIV,    "/", line);
    if (c == '=') return Token(TokenType::OP_ASSIGN, "=", line);
    if (c == '<') return Token(TokenType::OP_LT,     "<", line);
    if (c == '>') return Token(TokenType::OP_GT,     ">", line);
    if (c == '(') return Token(TokenType::LPAREN,    "(", line);
    if (c == ')') return Token(TokenType::RPAREN,    ")", line);
    if (c == '{') return Token(TokenType::LBRACE,    "{", line);
    if (c == '}') return Token(TokenType::RBRACE,    "}", line);
    if (c == ';') return Token(TokenType::SEMICOLON, ";", line);
    if (c == ',') return Token(TokenType::COMMA,     ",", line);

    return Token(TokenType::UNKNOWN, string(1, c), line);
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    while (true) {
        skipWhitespaceAndComments();

        if (pos >= source.size()) {
            tokens.push_back(Token(TokenType::END_OF_FILE, "EOF", line));
            break;
        }

        char c = current();

        if (isdigit(c)) {
            tokens.push_back(readNumber());
        }
        else if (isalpha(c) || c == '_') {
            tokens.push_back(readIdentifierOrKeyword());
        }
        else {
            tokens.push_back(readOperatorOrDelimiter());
        }
    }

    return tokens;
}