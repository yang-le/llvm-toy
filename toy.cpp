#include "scanner.hpp"
#include "parser.hpp"

#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static void HandleDefinition() {
    if (auto FnAST = ParseDefinition()) {
        if (auto *FnIR = FnAST->codegen()) {
            fprintf(stderr, "Read function definition:");
            FnIR->print(errs());
            fprintf(stderr, "\n");          
        }
    } else {
        getNextToken();
    }
}

static void HandleExtern() {
    if (auto ProtoAST = ParseExtern()) {
        if (auto *FnIR = ProtoAST->codegen()) {
            fprintf(stderr, "Read extern: ");
            FnIR->print(errs());
            fprintf(stderr, "\n");
        }
    } else {
        getNextToken();
    }
}

static void HandleTopLevelExpression() {
    if (auto FnAST = ParseTopLevelExpr()) {
        if (auto *FnIR = FnAST->codegen()) {
            fprintf(stderr, "Read top-level expression:");
            FnIR->print(errs());
            fprintf(stderr, "\n");
        }
    } else {
        getNextToken();
    }
}

static void MainLoop() {
    while(1) {
        fprintf(stderr, "ready> ");
        switch(getToken()) {
        case tok_eof:
            return;
        case ';':
            getNextToken();
            break;
        case tok_def:
            HandleDefinition();
            break;
        case tok_extern:
            HandleExtern();
            break;
        default:
            HandleTopLevelExpression();
            break;
        }
    }
}

//-----------------------------------------------------------------
// Main
//-----------------------------------------------------------------

int main() {
  // Install standard binary operators.
  // 1 is lowest precedence.
  setBinopPrecedence('<', 10);
  setBinopPrecedence('+', 20);
  setBinopPrecedence('-', 20);
  setBinopPrecedence('*', 40);  // highest.

  fprintf(stderr, "ready> ");
  getNextToken();
  
  initModule();

  MainLoop();

  printModule();

  return 0;
}
