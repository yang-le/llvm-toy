#ifndef _SCANNER_HPP
#define _SCANNER_HPP

#include <string>

enum Token {
    tok_eof = -1,

    // key words
    tok_def = -2,
    tok_extern = -3,

    tok_identifier = -4,
    tok_number = -5,
};

int getToken();
int getNextToken();
double getNumber();
std::string getIdentifier();

#endif  //_SCANNER_HPP
