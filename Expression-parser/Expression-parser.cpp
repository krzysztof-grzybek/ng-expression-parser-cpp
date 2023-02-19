#include <iostream>
#include "Chars.h"
#include "Lexer.h"

class UnquotedChars {
    std::string value;
    std::string::iterator current;
    std::string::iterator end;
    char currentQuote = -1;
    int escapeCount = 0;

public:

    UnquotedChars(std::string _value, short int start = 0) : value(_value), current(value.begin()), end(value.end()) {
        current += start;
    };


    char next() {
        int returnCode = 0;

        while (current != end) {

            int currentCode = static_cast<int>(*current);

            if (Chars::isQuote(currentCode) && (currentQuote == -1 || currentQuote == currentCode) && escapeCount % 2 == 0) {
                currentQuote = currentQuote == -1 ? currentCode : -1;
            }
            else if (currentQuote == -1) {
                returnCode = currentCode;
            }

            escapeCount = currentCode == Chars::$BACKSLASH ? escapeCount + 1 : 0;

            ++current;

            if (returnCode != 0) {
                return static_cast<char>(returnCode);
            }
        }

        return static_cast<char>(returnCode);
    }
};

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

    UnquotedChars a{ "startelo", 2 };

    while (auto curr = a.next()) {
        std::cout << curr;
    }

}

