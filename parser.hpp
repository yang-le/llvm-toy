#ifndef _PARSER_HPP
#define _PARSER_HPP

#include "ast.hpp"

std::unique_ptr<PrototypeAST> ParseExtern();
std::unique_ptr<FunctionAST> ParseDefinition();
std::unique_ptr<FunctionAST> ParseTopLevelExpr();

void setBinopPrecedence(char op, int prec);

#endif //_PARSER_HPP
