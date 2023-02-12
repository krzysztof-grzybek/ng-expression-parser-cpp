#pragma once
#include <iostream>
#include <vector>
#include "Chars.h"

namespace Lexer {
    std::string fromCharCode(int charCode);
    int charCodeAt(std::string subject, int pos);

    enum TokenType {
        Character,
        Identifier,
        PrivateIdentifier,
        Keyword,
        String,
        Operator,
        Number,
        Error
    };
    const std::string KEYWORDS[]{ "var", "let", "as", "null", "undefined", "true", "false", "if", "else", "this" };
    class Token {
    public:
        Token(const int index, const int end, const TokenType type, const float numValue, const std::string strValue);

        int index;
        const int end;
        const TokenType type;
        const float numValue;
        const std::string strValue;

        bool isCharacter(const short int  code);

        bool isNumber();

        bool isString();

        bool isOperator(const std::string _operator);

        bool isIdentifier();

        bool isPrivateIdentifier();

        bool isKeyword();

        bool isKeywordLet();

        bool isKeywordAs();

        bool isKeywordNull();

        bool isKeywordUndefined();

        bool isKeywordTrue();

        bool isKeywordFalse();

        bool isKeywordThis();

        bool isError();

        float toNumber();

        // TODO: return value is string | null
        std::string toString();
        std::string tokenTypeToString();
    };

    Token newCharacterToken(int index, int end, int code);

    Token newIdentifierToken(int index, int end, std::string text);

    Token newPrivateIdentifierToken(int index, int end, std::string text);

    Token newKeywordToken(int index, int end, std::string text);

    Token newOperatorToken(int index, int end, std::string text);

    Token newStringToken(int index, int end, std::string text);

    Token newNumberToken(int index, int end, int n);

    Token newErrorToken(int index, int end, std::string message);

    Token _EOF(-1, -1, TokenType::Character, 0, "");

    class Lexer {
    public:
        std::vector<Token> tokenize(std::string text);
    };

    bool _isIdentifierStart(const short int  code);

    bool isIdentifier(const std::string& input);

    bool _isIdentifierPart(const short int  code);

    bool _isExponentStart(const short int  code);

    bool _isExponentSign(const short int  code);

    const short int unescape(const short int code);

    int _parseIntAutoRadix(std::string text);
}