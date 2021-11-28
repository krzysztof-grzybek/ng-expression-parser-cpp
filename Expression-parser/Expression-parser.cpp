#include <iostream>
#include "Chars.h"
#include "Lexer.h"

int main()
{
    Lexer::Lexer lexer;
    std::vector<Lexer::Token> tokens = lexer.tokenize("let myVar = onMyEvent($event + 20); callMyFunc(a ?? \"ELOO\"); const o = b?.myProp ?? 4.3452");

    std::cout << "TOKEN LENGTH: \n" << tokens.size() << "\n";
    for (Lexer::Token currToken : tokens) {
        std::cout << "TOKEN TYPE: " << currToken.tokenTypeToString() << "\n";
        std::cout << "STR VAL: " << currToken.strValue << "\n";
        std::cout << "NUM VAL: " << currToken.numValue << "\n";
        std::cout << "---------------\n";
    }

}

