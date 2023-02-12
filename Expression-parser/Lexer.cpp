#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <regex>
#include <optional>
#include "Chars.h"
#include "Lexer.h"

using namespace std::literals::string_literals;

namespace Lexer {
    // TODO: ASCII now, should be UTF 16?
    int charCodeAt(std::string subject, int pos) {
        if (subject.size() <= pos) {
            throw "Out of range";
        }

        char character{ subject.at(pos) };
        return static_cast<int>(character);
    }

    // TODO: ASCII now, should be UTF 16?
    std::string fromCharCode(int charCode) {
        std::string result;
        char _char = static_cast<char>(charCode);
        result.push_back(_char);
        return result;
    }

    Token::Token(const int index, const int end, const TokenType type, const float numValue, const std::string strValue) : index{ index }, end{ end }, type{ type }, numValue{ numValue }, strValue{ strValue } {}
    bool Token::isCharacter(const short int  code) {
        return type == TokenType::Character && numValue == code;
    }

    bool Token::isNumber() {
        return type == TokenType::Number;
    }

    bool Token::isString() {
        return type == TokenType::String;
    }

    bool Token::isOperator(const std::string _operator) {
        return type == TokenType::Operator && strValue == _operator;
    }

    bool Token::isIdentifier() {
        return type == TokenType::Identifier;
    }

    bool Token::isPrivateIdentifier() {
        return type == TokenType::PrivateIdentifier;
    }

    bool Token::isKeyword() {
        return type == TokenType::Keyword;
    }

    bool Token::isKeywordLet() {
        return type == TokenType::Keyword && strValue == "let";
    }

    bool Token::isKeywordAs() {
        return type == TokenType::Keyword && strValue == "as";
    }

    bool Token::isKeywordNull() {
        return type == TokenType::Keyword && strValue == "null";
    }

    bool Token::isKeywordUndefined() {
        return type == TokenType::Keyword && strValue == "undefined";
    }

    bool Token::isKeywordTrue() {
        return type == TokenType::Keyword && strValue == "true";
    }

    bool Token::isKeywordFalse() {
        return type == TokenType::Keyword && strValue == "false";
    }

    bool Token::isKeywordThis() {
        return type == TokenType::Keyword && strValue == "this";
    }

    bool Token::isError() {
        return type == TokenType::Error;
    }

    float Token::toNumber() {
        return type == TokenType::Number ? numValue : -1;
    }

    std::string Token::toString() {
        switch (type) {
            case TokenType::Character:
            case TokenType::Identifier:
            case TokenType::Keyword:
            case TokenType::Operator:
            case TokenType::PrivateIdentifier:
            case TokenType::String:
            case TokenType::Error: {
                return strValue;
            }
            case TokenType::Number: {
                return std::to_string(numValue);
            }
            default: {
                return "";
            }
        }
    }

    std::string Token::tokenTypeToString() {
        switch (type) {
        case TokenType::Character:
            return "Character";
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::Keyword:
            return "Keyword";
        case TokenType::Operator:
            return "Operator";
        case TokenType::PrivateIdentifier:
            return "PrivateIdentifier";
        case TokenType::String:
            return "String";
        case TokenType::Error:
            return "Error";
        case TokenType::Number:
            return "Number";
        default:
            return "";
        }
    }

    Token newCharacterToken(int index, int end, int code) {
        return Token(index, end, TokenType::Character, code, fromCharCode(code));
    }

     Token newIdentifierToken(int index, int end, std::string text) {
          return Token(index, end, TokenType::Identifier, 0, text);
    }

     Token newPrivateIdentifierToken(int index, int end, std::string text) {
          return Token(index, end, TokenType::PrivateIdentifier, 0, text);
    }

     Token newKeywordToken(int index, int end, std::string text) {
          return Token(index, end, TokenType::Keyword, 0, text);
    }

     Token newOperatorToken(int index, int end, std::string text) {
          return Token(index, end, TokenType::Operator, 0, text);
    }

     Token newStringToken(int index, int end, std::string text) {
          return Token(index, end, TokenType::String, 0, text);
    }

     Token newNumberToken(int index, int end, float n) {
          return Token(index, end, TokenType::Number, n, "");
    }

    Token newErrorToken(int index, int end, std::string message) {
        return Token(index, end, TokenType::Error, 0, message);
    }

    class _Scanner {
        int length;
        int index = -1;

    public:
        int peek = 0;
        std::string input;
        _Scanner(std::string _input): input{_input} {
            length = input.size();
            advance();
        }

        void advance() {
            peek = ++index >= length ? Chars::$EOF : charCodeAt(input, index);
        }
  
        std::optional<Token> scanToken() {
            std::string inputLocal = input;
            int lengthLocal = length;
            int peekLocal = peek;
            int indexLocal = index;

            // Skip whitespace.
            while (peekLocal <= Chars::$SPACE) {
                if (++indexLocal >= length) {
                    peekLocal = Chars::$EOF;
                    break;
                }
                else {
                    peekLocal = charCodeAt(inputLocal, indexLocal);
                }
            }

             peek = peekLocal;
             index = indexLocal;

            if (indexLocal >= lengthLocal) {
                return std::nullopt;
            }

            // Handle identifiers and numbers.
            if (_isIdentifierStart(peek)) return scanIdentifier();
            if (Chars::isDigit(peek)) return scanNumber(index);

            int start = index;
            switch (peek) {
                case Chars::$PERIOD:
                    advance();
                    return Chars::isDigit(peek) ? scanNumber(start) : newCharacterToken(start, index, Chars::$PERIOD);
                case Chars::$LPAREN:
                case Chars::$RPAREN:
                case Chars::$LBRACE:
                case Chars::$RBRACE:
                case Chars::$LBRACKET:
                case Chars::$RBRACKET:
                case Chars::$COMMA:
                case Chars::$COLON:
                case Chars::$SEMICOLON:
                    return scanCharacter(start, peek);
                case Chars::$SQ:
                case Chars::$DQ:
                    return scanString();
                case Chars::$HASH:
                    return scanPrivateIdentifier();
                case Chars::$PLUS:
                case Chars::$MINUS:
                case Chars::$STAR:
                case Chars::$SLASH:
                case Chars::$PERCENT:
                case Chars::$CARET:
                    return scanOperator(start, fromCharCode(peek));
                case Chars::$QUESTION:
                    return scanQuestion(start);
                case Chars::$LT:
                case Chars::$GT:
                    return scanComplexOperator(start, fromCharCode(peek), Chars::$EQ, "=");
                case Chars::$BANG:
                case Chars::$EQ:
                    return scanComplexOperator(start, fromCharCode(peek), Chars::$EQ, "=", Chars::$EQ, "=");
                case Chars::$AMPERSAND:
                    return scanComplexOperator(start, "&", Chars::$AMPERSAND, " & ");
                case Chars::$BAR:
                    return scanComplexOperator(start, "|", Chars::$BAR, " | ");
                case Chars::$NBSP:
                    while (Chars::isWhitespace(peek)) advance();
                    return scanToken();
            }

            advance();
            std::string msgpPart1 = "Unexpected character[";
            std::string msgPart2 = fromCharCode(peek) + "]";
            std::string msg = msgpPart1 + msgPart2;
            return error(msg, 0);
        }

        Token scanCharacter(int start, int code) {
              advance();
              return newCharacterToken(start, index, code);
        }

        Token scanOperator(int start, std::string str) {
            advance();
            return newOperatorToken(start, index, str);
        }

        /**
        * Tokenize a 2/3 char long operator
        *
        * @param start start index in the expression
        * @param one first symbol (always part of the operator)
        * @param twoCode code point for the second symbol
        * @param two second symbol (part of the operator when the second code point matches)
        * @param threeCode code point for the third symbol
        * @param three third symbol (part of the operator when provided and matches source expression)
        */
        Token scanComplexOperator(int start, std::string one, int twoCode, std::string two, int threeCode = -1, std::string three = "A") {
            advance();
            std::string str = one;
            if (peek == twoCode) {
                advance();
                str += two;
            }
            if (threeCode != -1 && peek == threeCode) {
                advance();
                str += three;
            }
            return newOperatorToken(start, index, str);
        }

        Token scanIdentifier() {
            int start = index;
            advance();
            while (_isIdentifierPart(peek)) {
                advance();
            }

            std::string str = input.substr(start, index - start);
            bool isKeyword = std::find(std::begin(KEYWORDS), std::end(KEYWORDS), str) != std::end(KEYWORDS);
            return isKeyword ? newKeywordToken(start, index, str) :  newIdentifierToken(start, index, str);
        }

            /** Scans an ECMAScript private identifier. */
        Token scanPrivateIdentifier() {
            int start = index;
            advance();
            if (!_isIdentifierStart(peek)) {
                return error("Invalid character[#]", -1);
            }
            while (_isIdentifierPart(peek)) {
                advance();
            }

            std::string identifierName = input.substr(start, index -start);
            return newPrivateIdentifierToken(start, index, identifierName);
        }


        Token scanNumber(int start) {
            bool simple = (index == start);
            bool hasSeparators = false;
            advance();  // Skip initial digit.
            while (true) {
                if (Chars::isDigit(peek)) {
                    // Do nothing.
                    }
                else if (peek == Chars::$_) {
                    // Separators are only valid when they're surrounded by digits. E.g. `1_0_1` is
                    // valid while `_101` and `101_` are not. The separator can't be next to the decimal
                    // point or another separator either. Note that it's unlikely that we'll hit a case where
                    // the underscore is at the start, because that's a valid identifier and it will be picked
                    // up earlier in the parsing. We validate for it anyway just in case.
                    if (!Chars::isDigit(charCodeAt(input, index - 1)) ||
                        !Chars::isDigit(charCodeAt(input, index + 1))) {
                        return error("Invalid numeric separator", 0);
                    }
                    hasSeparators = true;
                }
                else if (peek == Chars::$PERIOD) {
                    simple = false;
                }
                else if (_isExponentStart(peek)) {
                    advance();
                    if (_isExponentSign(peek)) advance();
                    if (!Chars::isDigit(peek)) return error("Invalid exponent", -1);

                    simple = false;
                } else {
                    break;
                }
                advance();
            }

            std::string str = input.substr(start, index - start);
            if (hasSeparators) {
                str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
            }
            float value = simple ? static_cast<float>(_parseIntAutoRadix(str)) : std::stof(str);
            return newNumberToken(start, index, value);
        }

        Token scanString() {
            int start = index;
            int quote = peek;
            advance();  // Skip initial quote.

            std::string buffer = "";
            int marker = index;
            std::string inputLocal = input;

            while (peek != quote) {
                if (peek == Chars::$BACKSLASH) {
                buffer += inputLocal.substr(marker, index - marker);
                advance();
                int unescapedCode;
                // Workaround for TS2.1-introduced type strictness
                peek = peek;

                if (peek == Chars::$u) {
                    // 4 character hex code for unicode character.
                    std::string hex = inputLocal.substr(index + 1, 4);
     
                    if (std::regex_match(hex, std::regex("^[0-9a-f]+$"))) {
                        unescapedCode = std::stoi(hex, nullptr, 16);
                    } else {
                        return error("Invalid unicode escape[\\u+" + hex + "}]", 0);
                    }

                    for (int i = 0; i < 5; i++) {
                        advance();
                    }
                  }
                  
                  else {
                      unescapedCode = unescape(peek);
                      advance();
                  }
                  buffer += fromCharCode(unescapedCode);
                  marker = index;
                } else if (peek == Chars::$EOF) {
                    return error("Unterminated quote", 0);
                } else {
                    advance();
                }
            }

            std::string last = inputLocal.substr(marker, index - marker);
            advance();  // Skip terminating quote.

            return newStringToken(start, index, buffer + last);
        }

        Token scanQuestion(int start) {
            advance();
            std::string str = "?";
            // Either `a ?? b` or 'a?.b'.
            if (peek == Chars::$QUESTION || peek == Chars::$PERIOD) {
                str += peek == Chars::$PERIOD ? "." : "?";
                advance();
            }
            return newOperatorToken(start, index, str);
        }

        Token error(std::string message, int offset){
            int position = index + offset;
            std::string stringPosition = std::to_string(position);
            std::string _message = "Lexer Error : " + message + " at column " + stringPosition + " in expression[ " + input + "]";
            return newErrorToken(position, index, _message);
        }
    };


    std::vector<Token> Lexer::tokenize(std::string text) {
        _Scanner scanner{ text };
        std::vector<Token> tokens;
        // TODO: how to do it more rational way? (e.g reassign the 'token' variable)
        while (true) {
            std::optional<Token> tempToken = scanner.scanToken();
            if (tempToken.has_value()) {
                tokens.push_back(tempToken.value());
            }
            else {
                break;
            }
            
        }
        return tokens;
    }

    bool _isIdentifierStart(const short int  code) {
        return (Chars::$a <= code && code <= Chars::$z) || (Chars::$A <= code && code <= Chars::$Z) || (code == Chars::$_) || (code == Chars::$$);
    }

    bool isIdentifier(const std::string& input) {
        if (input.length() == 0) return false;

        _Scanner scanner{ input };
        if (!_isIdentifierStart(scanner.peek)) return false;
        scanner.advance();
        while (scanner.peek != Chars::$EOF) {
            if (!_isIdentifierPart(scanner.peek)) return false;
            scanner.advance();
        }
        return true;
    }

    bool _isIdentifierPart(const short int  code) {
        return Chars::isAsciiLetter(code) || Chars::isDigit(code) || (code == Chars::$_) || (code == Chars::$$);
    }

    bool _isExponentStart(const short int  code) {
        return code == Chars::$e || code == Chars::$E;
    }

    bool _isExponentSign(const short int  code) {
        return code == Chars::$MINUS || code == Chars::$PLUS;
    }

    const short int unescape(const short int code) {
        switch (code) {
        case Chars::$n:
            return Chars::$LF;
        case Chars::$f:
            return Chars::$FF;
        case Chars::$r:
            return Chars::$CR;
        case Chars::$t:
            return Chars::$TAB;
        case Chars::$v:
            return Chars::$VTAB;
        default:
            return code;
        }
    }

    int _parseIntAutoRadix(std::string text) {
        // TODO: rethrow exception - 'Invalid integer literal when parsing ' + text
        return std::stoi(text);
    }

    std::string safeCharAt(std::string subject, int index) {
        if (subject.size() <= index) {
            return "";
        }

        std::string result{ subject.at(index) };
        return result;
    }
}
