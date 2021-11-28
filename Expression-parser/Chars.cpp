#include "Chars.h";

bool Chars::isWhitespace(const short int code) {
    return (code >= Chars::$TAB && code <= Chars::$SPACE) || (code == Chars::$NBSP);
}

bool Chars::isDigit(const short int code) {
    return Chars::$0 <= code && code <= Chars::$9;
}

bool Chars::isAsciiLetter(const short int code) {
    return code >= Chars::$a && code <= Chars::$z || code >= Chars::$A && code <= Chars::$Z;
}

bool Chars::isAsciiHexDigit(const short int code) {
    return code >= Chars::$a && code <= Chars::$f || code >= Chars::$A && code <= Chars::$F || Chars::isDigit(code);
}

bool Chars::isNewLine(const short int code) {
    return code == Chars::$LF || code == Chars::$CR;
}

bool Chars::isOctalDigit(const short int code) {
    return Chars::$0 <= code && code <= Chars::$7;
}

bool Chars::isQuote(const short int code) {
    return code == Chars::$SQ || code == Chars::$DQ || code == Chars::$BT;
}


