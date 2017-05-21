#ifndef _COMMON_HPP
#define _COMMON_HPP

#include "ast.hpp"

std::unique_ptr<ExprAST> LogError(const char *Str);
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);
llvm::Value *LogErrorV(const char *Str);

#endif  //_COMMON_HPP
