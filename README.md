# Secure-Mini-Compiler-v1.0
A lightweight compiler in C++ that performs lexical analysis, parsing, semantic analysis, and static security checks (division by zero, uninitialized variables, infinite loops) with an optional Qt GUI.


# 🔐 Secure Mini Compiler

A lightweight compiler built in **C++** that translates a simplified programming language while catching security vulnerabilities at compile time. This project was built as part of a Compiler Construction course and covers all major phases of a compiler — from lexical analysis to static security analysis — with an optional **Qt-based GUI**.

---

## 📋 Table of Contents

- [What This Project Does](#what-this-project-does)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [How to Use (Terminal Mode)](#how-to-use-terminal-mode)
- [How to Use (GUI Mode)](#how-to-use-gui-mode)
- [Writing Your Own Test File](#writing-your-own-test-file)
- [What Each Phase Does](#what-each-phase-does)
- [What the Security Analyzer Catches](#what-the-security-analyzer-catches)
- [File Reference](#file-reference)

---

## What This Project Does

Most compilers just check if your code is syntactically correct. This one goes further — it also checks if your code is **safe**. It runs your source file through four phases:

1. **Lexical Analysis** — Breaks your code into tokens
2. **Parsing** — Checks the grammar and builds an AST (Abstract Syntax Tree)
3. **Semantic Analysis** — Makes sure variables are declared, types match, etc.
4. **Security Analysis** — Catches division by zero, uninitialized variables, and infinite loops

You can run it through the **terminal** or through a **graphical interface** built with Qt.

---

## Project Structure

```
SecureMiniCompiler/
│
├── main.cpp                  # Terminal entry point
├── main_gui.cpp              # GUI entry point (Qt)
├── compiler.pro              # Qt project file
├── test.smc                  # Sample source file to test the compiler
│
├── lexer/
│   ├── lexer.h               # Token types and Lexer class declaration
│   └── lexer.cpp             # Tokenizer implementation
│
├── parser/
│   ├── parser.h              # Parser class declaration
│   └── parser.cpp            # Recursive descent parser
│
├── ast/
│   └── ast.h                 # All AST node definitions
│
├── semantic/
│   ├── analyzer.h            # Semantic analyzer declaration
│   └── analyzer.cpp          # Scope checking, type checking
│
├── security/
│   ├── security.h            # Security analyzer declaration
│   └── security.cpp          # Static analysis checks
│
└── utils/                    # Reserved for future utilities
```

---

## Requirements

### For Terminal Mode
- A C++ compiler that supports **C++17**
- Recommended: `g++` via MinGW (Windows) or GCC (Linux/Mac)

### For GUI Mode
- **Qt 6.x** — Download from [https://www.qt.io/download-qt-installer](https://www.qt.io/download-qt-installer)
- During Qt installation make sure you select:
  - Qt 6.x → MinGW 64-bit
  - Qt Creator

---

## Getting Started

### Step 1 — Clone the Repository

```bash
git clone https://github.com/your-username/SecureMiniCompiler.git
cd SecureMiniCompiler
```

### Step 2 — Choose Your Mode

There are two ways to run this compiler. Pick whichever works for you:

- **Terminal Mode** — Simpler, no extra installs needed
- **GUI Mode** — Requires Qt, but gives a visual interface with phase indicators

---

## How to Use (Terminal Mode)

### Compile

```bash
g++ -std=c++17 main.cpp lexer/lexer.cpp parser/parser.cpp semantic/analyzer.cpp security/security.cpp -o compiler
```

### Run

```bash
./compiler test.smc
```

On Windows:

```bash
.\compiler.exe test.smc
```

### Expected Output

```
==========================================
       Secure Mini Compiler v1.0
==========================================

Source file loaded: test.smc

[ Phase 1 ] Lexical Analysis...
  Tokens generated: 52

[ Phase 2 ] Parsing...
  Statements found: 7

[ Phase 3 ] Semantic Analysis...
Semantic analysis passed with no errors.

[ Phase 4 ] Security Analysis...

===== Security Analysis Report =====
[ERROR] variable 'z' is used before being initialized
[ERROR] division by zero detected
[WARNING] infinite loop detected, while condition is always true
====================================
Total issues found: 3

==========================================
        Compilation Complete
==========================================
```

---

## How to Use (GUI Mode)

### Option A — Using Qt Creator (Recommended)

1. Open **Qt Creator**
2. Go to `File` → `Open File or Project`
3. Select `compiler.pro` from the project folder
4. When asked to configure, select **Desktop Qt 6.x MinGW 64-bit**
5. Click **Configure Project**
6. Press the green **Run button** or `Ctrl + R`
7. The GUI window will open
8. Click **Browse** to select your `.smc` file
9. Click **Compile & Analyze**

### Option B — Using Terminal with Qt

Make sure Qt's MinGW is in your PATH first:

```bash
set PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.0\mingw_64\bin;%PATH%
```

Then:

```bash
qmake compiler.pro
mingw32-make
.\release\SecureMiniCompiler.exe
```

### What the GUI Shows

- A file picker to load your `.smc` source file
- Four phase indicator boxes that turn **green** (pass), **red** (error), or **yellow** (warning)
- A live output console showing all results
- Color coded error and warning messages

---

## Writing Your Own Test File

Create a file with a `.smc` extension. The language supports:

| Feature | Example |
|---|---|
| Integer variable | `int x = 10;` |
| Float variable | `float y = 3.14;` |
| Assignment | `x = x + 1;` |
| If statement | `if (x > 0) { ... }` |
| If-else | `if (x > 0) { ... } else { ... }` |
| While loop | `while (x > 0) { ... }` |
| Return | `return x;` |
| Operators | `+  -  *  /  ==  !=  <  >  <=  >=` |
| Comments | `// this is a comment` |

### Sample Test File with Intentional Errors

```
int x = 10;
float y = 3.14;
int z;
int a = z + 5;
int b = x / 0;
int c = y + 1;
while (1 > 0) {
    x = x + 1;
}
return x;
```

This file will trigger all four types of issues the compiler can catch.

---

## What Each Phase Does

### Phase 1 — Lexical Analysis
Reads the source file character by character and converts it into a list of tokens. A token is the smallest meaningful unit — a keyword, variable name, number, or operator.

### Phase 2 — Parsing
Takes the token list and checks if the structure of the code is grammatically correct. Also builds an **Abstract Syntax Tree (AST)** which is a tree representation of your entire program.

### Phase 3 — Semantic Analysis
Walks through the AST and checks:
- Variables are declared before use
- No variable is declared twice in the same scope
- Type compatibility (assigning float to int triggers a warning)

### Phase 4 — Security Analysis
Walks through the AST again looking for dangerous patterns:
- **Uninitialized variable usage** — variable used before being given a value
- **Division by zero** — literal zero on the right side of a `/` operator
- **Infinite loops** — `while` condition that is always mathematically true

---

## What the Security Analyzer Catches

| Issue | Severity | Example |
|---|---|---|
| Uninitialized variable | ERROR | `int x; int y = x + 1;` |
| Division by zero | ERROR | `int z = a / 0;` |
| Infinite loop | WARNING | `while (1 > 0) { ... }` |

---

## File Reference

| File | Purpose |
|---|---|
| `main.cpp` | Terminal mode entry point, chains all four phases |
| `main_gui.cpp` | Qt GUI entry point, same phases with visual output |
| `compiler.pro` | Qt build configuration file |
| `lexer/lexer.h` | Token types enum and Lexer class header |
| `lexer/lexer.cpp` | Full tokenizer implementation |
| `parser/parser.h` | Parser class header |
| `parser/parser.cpp` | Recursive descent parser implementation |
| `ast/ast.h` | All AST node structs (VarDecl, If, While, BinaryOp, etc.) |
| `semantic/analyzer.h` | Semantic analyzer header |
| `semantic/analyzer.cpp` | Scope stack, type checking, variable tracking |
| `security/security.h` | Security analyzer header with SecurityIssue struct |
| `security/security.cpp` | Static analysis checks implementation |
| `test.smc` | Sample source file for testing |

---

## Built With

- **C++17**
- **Qt 6.11** (GUI only)
- **Qt Creator** (IDE)

---

## Author

**Rehan Khan**
Built as a 6th semester Compiler Construction course project.

---

© 2026 Rehan Khan. All rights reserved.
Unauthorized copying, distribution, or modification of this project without explicit permission is prohibited.
