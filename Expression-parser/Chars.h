#pragma once
namespace Chars {
    // TODO: consider those ints "extern", if will be used in multiple translation units
    const int $EOF{ 0 };
    const int $BSPACE{ 8 };
    const int $TAB{ 9 };
    const int $LF{ 10 };
    const int $VTAB{ 11 };
    const int $FF{ 12 };
    const int $CR{ 13 };
    const int $SPACE{ 32 };
    const int $BANG{ 33 };
    const int $DQ{ 34 };
    const int $HASH{ 35 };
    const int $${ 36 };
    const int $PERCENT{ 37 };
    const int $AMPERSAND{ 38 };
    const int $SQ{ 39 };
    const int $LPAREN{ 40 };
    const int $RPAREN{ 41 };
    const int $STAR{ 42 };
    const int $PLUS{ 43 };
    const int $COMMA{ 44 };
    const int $MINUS{ 45 };
    const int $PERIOD{ 46 };
    const int $SLASH{ 47 };
    const int $COLON{ 58 };
    const int $SEMICOLON{ 59 };
    const int $LT{ 60 };
    const int $EQ{ 61 };
    const int $GT{ 62 };
    const int $QUESTION{ 63 };

    const int $0{ 48 };
    const int $7{ 55 };
    const int $9{ 57 };

    const int $A{ 65 };
    const int $E{ 69 };
    const int $F{ 70 };
    const int $X{ 88 };
    const int $Z{ 90 };

    const int $LBRACKET{ 91 };
    const int $BACKSLASH{ 92 };
    const int $RBRACKET{ 93 };
    const int $CARET{ 94 };
    const int $_{ 95 };

    const int $a{ 97 };
    const int $b{ 98 };
    const int $e{ 101 };
    const int $f{ 102 };
    const int $n{ 110 };
    const int $r{ 114 };
    const int $t{ 116 };
    const int $u{ 117 };
    const int $v{ 118 };
    const int $x{ 120 };
    const int $z{ 122 };

    const int $LBRACE{ 123 };
    const int $BAR{ 124 };
    const int $RBRACE{ 125 };
    const int $NBSP{ 160 };

    const int $PIPE{ 124 };
    const int $TILDA{ 126 };
    const int $AT{ 64 };

    const int $BT{ 96 };

    bool isWhitespace(const short int code);

    bool isDigit(const short int code);

    bool isAsciiLetter(const short int code);

    bool isAsciiHexDigit(const short int code);

    bool isNewLine(const short int code);

    bool isOctalDigit(const short int code);

    bool isQuote(const short int code);
}

