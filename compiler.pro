QT += core gui widgets

CONFIG += c++17

TARGET = SecureMiniCompiler
TEMPLATE = app

SOURCES += \
    main_gui.cpp \
    lexer/lexer.cpp \
    parser/parser.cpp \
    semantic/analyzer.cpp \
    security/security.cpp

HEADERS += \
    lexer/lexer.h \
    parser/parser.h \
    ast/ast.h \
    semantic/analyzer.h \
    security/security.h

INCLUDEPATH += .