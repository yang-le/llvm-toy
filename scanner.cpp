#include <cstdlib>
#include <cstdio>
#include "scanner.hpp"

static std::string IdentifierStr;
static double NumVal;
static int Token;

std::string getIdentifier() {
    return IdentifierStr;
}

double getNumber() {
    return NumVal;
}

int getToken() {
    return Token;
}

int getNextToken() {
    static int LastChar = ' ';

    while(isspace(LastChar))
        LastChar = getchar();
    
    if (isalpha(LastChar)) {
        IdentifierStr = LastChar;
        while(isalnum((LastChar = getchar())))
            IdentifierStr += LastChar;
        
        if (IdentifierStr == "def")
            return (Token = tok_def);
        if (IdentifierStr == "extern")
            return (Token = tok_extern);
        return (Token = tok_identifier);
    }

    if (isdigit(LastChar) || LastChar == '.') {
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while(isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), 0);
        return (Token = tok_number);
    }

    if (LastChar == '#') {
        do
            LastChar = getchar();
        while(LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return getNextToken();
    }

    if (LastChar == EOF)
        return (Token = tok_eof);
    
    Token = LastChar;
    LastChar = getchar();
    return Token;
}
