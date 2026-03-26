#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFrame>
#include <QFont>
#include <QScrollArea>
#include <fstream>
#include <sstream>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/analyzer.h"
#include "security/security.h"

using namespace std;

// reads file into a string
string readFile(const string& path) {
    ifstream file(path);
    if (!file.is_open())
        throw runtime_error("Could not open file: " + path);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ── Main Window ───────────────────────────────────────
class CompilerWindow : public QWidget {
public:
    CompilerWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("Secure Mini Compiler v1.0");
        setMinimumSize(750, 650);
        setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(12);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        // title
        QLabel* title = new QLabel("Secure Mini Compiler v1.0");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 20px; font-weight: bold; color: #cba6f7; padding: 10px;");
        mainLayout->addWidget(title);

        // separator
        mainLayout->addWidget(makeLine());

        // file picker row
        QHBoxLayout* fileRow = new QHBoxLayout();
        filePathInput = new QLineEdit();
        filePathInput->setPlaceholderText("Select your .smc source file...");
        filePathInput->setReadOnly(true);
        filePathInput->setStyleSheet(
            "background-color: #313244; color: #cdd6f4;"
            "border: 1px solid #45475a; border-radius: 6px; padding: 6px 10px; font-size: 13px;"
        );

        QPushButton* browseBtn = new QPushButton("Browse");
        browseBtn->setFixedWidth(90);
        browseBtn->setStyleSheet(
            "background-color: #45475a; color: #cdd6f4;"
            "border: none; border-radius: 6px; padding: 7px; font-size: 13px;"
            "hover { background-color: #585b70; }"
        );

        fileRow->addWidget(filePathInput);
        fileRow->addWidget(browseBtn);
        mainLayout->addLayout(fileRow);

        // compile button
        compileBtn = new QPushButton("Compile & Analyze");
        compileBtn->setEnabled(false);
        compileBtn->setStyleSheet(
            "background-color: #cba6f7; color: #1e1e2e;"
            "border: none; border-radius: 8px; padding: 10px; font-size: 15px; font-weight: bold;"
        );
        mainLayout->addWidget(compileBtn);

        // separator
        mainLayout->addWidget(makeLine());

        // phase indicators
        QLabel* phasesTitle = new QLabel("Compilation Phases");
        phasesTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #89b4fa;");
        mainLayout->addWidget(phasesTitle);

        QHBoxLayout* phasesRow = new QHBoxLayout();
        phase1Label = makePhaseLabel("Phase 1\nLexical Analysis");
        phase2Label = makePhaseLabel("Phase 2\nParsing");
        phase3Label = makePhaseLabel("Phase 3\nSemantic Analysis");
        phase4Label = makePhaseLabel("Phase 4\nSecurity Analysis");

        phasesRow->addWidget(phase1Label);
        phasesRow->addWidget(phase2Label);
        phasesRow->addWidget(phase3Label);
        phasesRow->addWidget(phase4Label);
        mainLayout->addLayout(phasesRow);

        // separator
        mainLayout->addWidget(makeLine());

        // output area
        QLabel* outputTitle = new QLabel("Output");
        outputTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #89b4fa;");
        mainLayout->addWidget(outputTitle);

        outputArea = new QTextEdit();
        outputArea->setReadOnly(true);
        outputArea->setStyleSheet(
            "background-color: #181825; color: #a6e3a1;"
            "border: 1px solid #45475a; border-radius: 6px;"
            "font-family: Consolas, monospace; font-size: 13px; padding: 8px;"
        );
        outputArea->setMinimumHeight(220);
        mainLayout->addWidget(outputArea);

        // clear button
        QPushButton* clearBtn = new QPushButton("Clear Output");
        clearBtn->setStyleSheet(
            "background-color: #313244; color: #cdd6f4;"
            "border: none; border-radius: 6px; padding: 7px; font-size: 13px;"
        );
        mainLayout->addWidget(clearBtn);

        // connections
        connect(browseBtn,  &QPushButton::clicked, this, &CompilerWindow::browseFile);
        connect(compileBtn, &QPushButton::clicked, this, &CompilerWindow::runCompiler);
        connect(clearBtn,   &QPushButton::clicked, this, &CompilerWindow::clearOutput);
    }

private:
    QLineEdit* filePathInput;
    QPushButton* compileBtn;
    QTextEdit* outputArea;
    QLabel* phase1Label;
    QLabel* phase2Label;
    QLabel* phase3Label;
    QLabel* phase4Label;

    // creates a horizontal separator line
    QFrame* makeLine() {
        QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("color: #45475a;");
        return line;
    }

    // creates a phase status box
    QLabel* makePhaseLabel(const QString& text) {
        QLabel* label = new QLabel(text);
        label->setAlignment(Qt::AlignCenter);
        label->setFixedHeight(60);
        label->setStyleSheet(
            "background-color: #313244; color: #6c7086;"
            "border: 1px solid #45475a; border-radius: 8px; font-size: 12px; padding: 5px;"
        );
        return label;
    }

    // resets all phase boxes to grey
    void resetPhases() {
        auto reset = [](QLabel* l) {
            l->setStyleSheet(
                "background-color: #313244; color: #6c7086;"
                "border: 1px solid #45475a; border-radius: 8px; font-size: 12px; padding: 5px;"
            );
        };
        reset(phase1Label);
        reset(phase2Label);
        reset(phase3Label);
        reset(phase4Label);
    }

    // marks a phase as passed (green)
    void setPhasePass(QLabel* label) {
        label->setStyleSheet(
            "background-color: #1e3a2f; color: #a6e3a1;"
            "border: 1px solid #a6e3a1; border-radius: 8px; font-size: 12px; padding: 5px; font-weight: bold;"
        );
    }

    // marks a phase as failed (red)
    void setPhaseFail(QLabel* label) {
        label->setStyleSheet(
            "background-color: #3a1e1e; color: #f38ba8;"
            "border: 1px solid #f38ba8; border-radius: 8px; font-size: 12px; padding: 5px; font-weight: bold;"
        );
    }

    // marks a phase as warning (yellow)
    void setPhaseWarn(QLabel* label) {
        label->setStyleSheet(
            "background-color: #3a3020; color: #f9e2af;"
            "border: 1px solid #f9e2af; border-radius: 8px; font-size: 12px; padding: 5px; font-weight: bold;"
        );
    }

    void log(const QString& msg) {
        outputArea->append(msg);
    }

    void browseFile() {
        QString path = QFileDialog::getOpenFileName(
            this, "Open Source File", "", "SMC Files (*.smc);;All Files (*)"
        );
        if (!path.isEmpty()) {
            filePathInput->setText(path);
            compileBtn->setEnabled(true);
            compileBtn->setStyleSheet(
                "background-color: #cba6f7; color: #1e1e2e;"
                "border: none; border-radius: 8px; padding: 10px; font-size: 15px; font-weight: bold;"
            );
        }
    }

    void clearOutput() {
        outputArea->clear();
        resetPhases();
    }

    void runCompiler() {
        outputArea->clear();
        resetPhases();

        string filePath = filePathInput->text().toStdString();
        string source;

        // read the file
        try {
            source = readFile(filePath);
        }
        catch (runtime_error& e) {
            log("❌ " + QString::fromStdString(e.what()));
            return;
        }

        log("📂 File loaded: " + filePathInput->text());
        log("─────────────────────────────────");

        // ── Phase 1: Lexical Analysis ──────
        log("\n🔍 [ Phase 1 ] Lexical Analysis...");
        Lexer lexer(source);
        vector<Token> tokens = lexer.tokenize();
        log("   Tokens generated: " + QString::number(tokens.size()));
        setPhasePass(phase1Label);
        log("   ✅ Passed");

        // ── Phase 2: Parsing ───────────────
        log("\n🌳 [ Phase 2 ] Parsing...");
        Parser parser(tokens);
        shared_ptr<ProgramNode> ast;

        try {
            ast = parser.parse();
            log("   Statements found: " + QString::number(ast->statements.size()));
            setPhasePass(phase2Label);
            log("   ✅ Passed");
        }
        catch (runtime_error& e) {
            log("   ❌ Parse Error: " + QString::fromStdString(e.what()));
            setPhaseFail(phase2Label);
            return;
        }

        // ── Phase 3: Semantic Analysis ─────
        log("\n🧠 [ Phase 3 ] Semantic Analysis...");
        try {
            SemanticAnalyzer analyzer;
            analyzer.analyze(ast);
            setPhasePass(phase3Label);
            log("   ✅ Passed");
        }
        catch (runtime_error& e) {
            log("   ❌ Semantic Error: " + QString::fromStdString(e.what()));
            setPhaseFail(phase3Label);
            return;
        }

        // ── Phase 4: Security Analysis ─────
        log("\n🔐 [ Phase 4 ] Security Analysis...");

        // temporarily capture security output
        SecurityAnalyzer security;
        security.analyze(ast);

        // get issues through a small helper
        auto issues = security.getIssues();

        if (issues.empty()) {
            setPhasePass(phase4Label);
            log("   ✅ No security issues found.");
        }
        else {
            bool hasError = false;
            for (auto& issue : issues) {
                if (issue.severity == "ERROR") hasError = true;
                QString color = (issue.severity == "ERROR") ? "#f38ba8" : "#f9e2af";
                QString icon  = (issue.severity == "ERROR") ? "❌" : "⚠️";
                outputArea->append(
                    "<span style='color:" + color + ";'>"
                    + icon + " [" + QString::fromStdString(issue.severity) + "] "
                    + QString::fromStdString(issue.message)
                    + "</span>"
                );
            }
            log("   Total issues: " + QString::number(issues.size()));
            hasError ? setPhaseFail(phase4Label) : setPhaseWarn(phase4Label);
        }

        log("\n─────────────────────────────────");
        log("✅ Compilation complete.");
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    CompilerWindow window;
    window.show();
    return app.exec();
}