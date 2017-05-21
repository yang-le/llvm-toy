#include "parser.hpp"
#include "common.hpp"
#include "scanner.hpp"
#include "llvm/ADT/STLExtras.h"

#include <map>

static std::map<char, int> BinopPrecedence;
static int GetTokPrecedence() {
    if (!isascii(getToken()))
        return -1;
    
    int TokPrec = BinopPrecedence[getToken()];
    if (TokPrec <= 0) return -1;
    return TokPrec;
}

static std::unique_ptr<ExprAST> ParseExpression();

static std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto Result = llvm::make_unique<NumberExprAST>(getNumber());
    getNextToken();
    return std::move(Result);
}

static std::unique_ptr<ExprAST> ParseParenExpr() {
    getNextToken();
    auto V = ParseExpression();
    if (!V)
        return nullptr;
    
    if (getToken() != ')')
        return LogError("expected ')'");
    
    getNextToken();
    return V;
}

static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string IdName = getIdentifier();

    getNextToken();

    if (getToken() != '(')
        return llvm::make_unique<VariableExprAST>(IdName);
    
    getNextToken();
    std::vector<std::unique_ptr<ExprAST>> Args;
    if (getToken() != ')') {
        while(1) {
            if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;

            if (getToken() == ')')
                break;
            
            if (getToken() != ',')
                return LogError("Expected ')' or ',' in argument list");

            getNextToken();
        }
    }

    getNextToken();

    return llvm::make_unique<CallExprAST>(IdName, std::move(Args));
}

static std::unique_ptr<ExprAST> ParsePrimary() {
    switch (getToken()) {
    default:
        return LogError("unknown token when expecting an expression");
    case tok_identifier:
        return ParseIdentifierExpr();
    case tok_number:
        return ParseNumberExpr();
    case '(':
        return ParseParenExpr();
    }
}

// ExprPrec : the minimal operator precedence that the function is allowed to eat.
static std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                                std::unique_ptr<ExprAST> LHS) {
    while(1) {
        int TokPrec = GetTokPrecedence();

        // if TokPrec is less than the minimal precedence, we are done.
        if (TokPrec < ExprPrec)
            return LHS;
        
        // otherwise, we consume it
        int BinOp = getToken();
        getNextToken();

        auto RHS = ParsePrimary();
        if (!RHS)
            return nullptr;
        
        // if next op has HIGHER precedence, handle them first
        int NextPrec = GetTokPrecedence();
        if (TokPrec < NextPrec) {
            // TokPrec + 1 to make sure it ONLY handle the HIGHER-precedence-op.
            RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }

        LHS = llvm::make_unique<BinaryExprAST>(BinOp, std::move(LHS),
                                                std::move(RHS));
    }
}

static std::unique_ptr<ExprAST> ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS)
        return nullptr;
    
    return ParseBinOpRHS(0, std::move(LHS));
}

static std::unique_ptr<PrototypeAST> ParsePrototype() {
    if (getToken() != tok_identifier)
        return LogErrorP("Expected funciotn name in prototype");
    
    std::string FnName = getIdentifier();
    getNextToken();

    if (getToken() != '(')
        return LogErrorP("Expected '(' in prototype");
    
    std::vector<std::string> ArgNames;
    while(getNextToken() == tok_identifier)
        ArgNames.push_back(getIdentifier());
    if (getToken() != ')')
        return LogErrorP("Expected ')' in prototype");
    
    getNextToken();

    return llvm::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

std::unique_ptr<FunctionAST> ParseDefinition() {
    getNextToken();
    auto Proto = ParsePrototype();
    if (!Proto) return nullptr;

    if (auto E = ParseExpression())
        return llvm::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    return nullptr;
}

std::unique_ptr<PrototypeAST> ParseExtern() {
    getNextToken();
    return ParsePrototype();
}

std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
    if (auto E = ParseExpression()) {
        auto Proto = llvm::make_unique<PrototypeAST>("", std::vector<std::string>());
        return llvm::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

void setBinopPrecedence(char op, int prec) {
    BinopPrecedence[op] = prec;
}
