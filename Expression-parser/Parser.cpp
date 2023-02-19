#include <algorithm>;
#include <cctype>
#include <map>
#include <functional>
#include "Parser.h";

enum ParseContextFlags {
    None = 0,
    /**
     * A Writable context is one in which a value may be written to an lvalue.
     * For example, after we see a property access, we may expect a write to the
     * property via the "=" operator.
     *   prop
     *        ^ possible "=" after
     */
     Writable = 1,
};

class _ParseAST {
private:
    int rparensExpected{ 0 };
    int rbracketsExpected{ 0 };
    int rbracesExpected{ 0 };
    ParseContextFlags context{ ParseContextFlags::None };

    template<typename T>
    T withContext(ParseContextFlags _context, std::function<T()> cb) {
        context |= _context;
        const ret = cb();
        context ^= _context;
        return ret;
    }

    std::string locationText(int _index = -1) { // -1 means no value
        if (_index == -1) _index = index;
        std::string column = std::to_string(tokens.at(_index).index + 1);
        return (_index < tokens.size()) ? "at column "s + column + " in" : "at the end of the expression"s;
    }

    /**
     * Records an error for an unexpected private identifier being discovered.
     * @param token Token representing a private identifier.
     * @param extraMessage Optional additional message being appended to the error.
     */
    void _reportErrorForPrivateIdentifier(Lexer::Token token, std::string extraMessage = "") {
        std::string errorMessage = "Private identifiers are not supported.Unexpected private identifier: "s + token.toString();
        if (extraMessage != "") {
            errorMessage += ", " + extraMessage;
        }
        error(errorMessage);
    }

    void skip() {
        while (index < tokens.size() && !n.isCharacter(Chars::$SEMICOLON) &&
            // Lexer::Token n = next();
            !n.isOperator("|") && (rparensExpected <= 0 || !n.isCharacter(Chars::$RPAREN)) &&
            (rbracesExpected <= 0 || !n.isCharacter(Chars::$RBRACE)) &&
            (rbracketsExpected <= 0 || !n.isCharacter(Chars::$RBRACKET)) &&
            (!(context & ParseContextFlags::Writable) || !n.isOperator("="))) {
            if (next().isError()) {
                errors.push_back(Ast::ParserError(next().toString(), input, locationText(), location));
            }
            advance();
            //n = next(); // TODO: how to do it properly?
        }
    }

    // Cache of expression start and input indeces to the absolute source span they map to, used to
    // prevent creating superfluous source spans in `sourceSpan`.
    // A serial of the expression start and input index is used for mapping because both are stateful
    // and may change for subsequent expressions visited by the parser.
    std::map<std::string, Ast::AbsoluteSourceSpan> sourceSpanCache;
public:
    int index{ 0 };
    int offset;
    int inputLength;
    int absoluteOffset;
    std::string input;
    std::string location;
    std::vector<Lexer::Token> tokens;
    std::vector<Ast::ParserError> errors;

    _ParseAST(
        std::string input, std::string location, int absoluteOffset,
        std::vector<Lexer::Token> tokens, int inputLength, bool parseAction,
        std::vector<Ast::ParserError> errors, int offset
    ) : tokens{ tokens }, offset{ offset }, inputLength{ inputLength }, absoluteOffset{ absoluteOffset }, errors{ errors }, input{ input }, location{ location }{}

    Lexer::Token peek(int offset) {
        int i = index + offset;
        return i < tokens.size() ? tokens.at(i) : Lexer::_EOF;
    }

    Lexer::Token next() {
        return peek(0);
    }

    /** Whether all the parser input has been processed. */
    bool atEOF() {
        return index >= tokens.size();
    }

    /**
    * Index of the next token to be processed, or the end of the last token if all have been
    * processed.
    */
    int inputIndex() {
        return atEOF() ? currentEndIndex() : next().index + offset;
    }

    /**
     * End index of the last processed token, or the start of the first token if none have been
     * processed.
     */
    int currentEndIndex() {
        if (index > 0) {
            Lexer::Token curToken = peek(-1);
            return curToken.end + offset;
        }
        // No tokens have been processed yet; return the next token's start or the length of the input
        // if there is no token.
        if (tokens.size() == 0) {
            return inputLength + offset;
        }
        return next().index + offset;
    }

    /**
     * Returns the absolute offset of the start of the current token.
     */
    int currentAbsoluteOffset() {
        return absoluteOffset + inputIndex();
    }

    /**
     * Retrieve a `ParseSpan` from `start` to the current position (or to `artificialEndIndex` if
     * provided).
     *
     * @param start Position from which the `ParseSpan` will start.
     * @param artificialEndIndex Optional ending index to be used if provided (and if greater than the
     *     natural ending index)
     */
    Ast::ParseSpan span(int start, int artificialEndIndex) { // artificialEndIndex -1 is means to be empty
        int endIndex = currentEndIndex();
        if (artificialEndIndex != -1 && artificialEndIndex > currentEndIndex()) {
            endIndex = artificialEndIndex;
        }

        // In some unusual parsing scenarios (like when certain tokens are missing and an `EmptyExpr` is
        // being created), the current token may already be advanced beyond the `currentEndIndex`. This
        // appears to be a deep-seated parser bug.
        //
        // As a workaround for now, swap the start and end indices to ensure a valid `ParseSpan`.
        // TODO(alxhub): fix the bug upstream in the parser state, and remove this workaround.
        if (start > endIndex) {
            int tmp = endIndex;
            endIndex = start;
            start = tmp;
        }

        return Ast::ParseSpan(start, endIndex);
    }


    Ast::AbsoluteSourceSpan sourceSpan(int start, int artificialEndIndex) { // artificialEndIndex -1 is means to be empty
        std::string startString = std::to_string(start);
        std::string inputIndexString = std::to_string(inputIndex());
        std::string artificialEndIndexString = std::to_string(artificialEndIndex);
        std::string serial = startString + "@" + inputIndexString + artificialEndIndexString;
        bool containsSerial = sourceSpanCache.find(serial) != sourceSpanCache.end();
        if (!containsSerial) {
            sourceSpanCache.insert({ serial, span(start, artificialEndIndex).toAbsolute(absoluteOffset) });
        }

        return sourceSpanCache.at(serial);
    }

    void advance() {
        index++;
    }

    bool consumeOptionalCharacter(int code) {
        if (next().isCharacter(code)) {
            advance();
            return true;
        }
        else {
            return false;
        }
    }

    bool peekKeywordLet() {
        return next().isKeywordLet();
    }
    bool peekKeywordAs() {
        return next().isKeywordAs();
    }

    void error(std::string message, int _index = -1) { // -1 means no value
        errors.push_back(Ast::ParserError(message, input, locationText(_index), location));
        skip();
    }

    /**
     * Consumes an expected character, otherwise emits an error about the missing expected character
     * and skips over the token stream until reaching a recoverable point.
     *
     * See `this.error` and `this.skip` for more details.
     */
    void expectCharacter(int code) {
        if (consumeOptionalCharacter(code)) return;
        error("Missing expected "s + Lexer::fromCharCode(code));
    }

    Ast::AST parseChain() {
        std::vector< Ast::AST> exprs;
        int start = inputIndex();
        /*
        while (index < tokens.size()) {
          const expr = .parsePipe();
          exprs.push(expr);

          if (this.consumeOptionalCharacter(chars.$SEMICOLON)) {
            if (!this.parseAction) {
              this.error('Binding expression cannot contain chained expression');
            }
            while (this.consumeOptionalCharacter(chars.$SEMICOLON)) {
            }  // read all semicolons
          }
              else if (this.index < this.tokens.length) {
              this.error(`Unexpected token '${this.next}'`);
              }
          }

          if (exprs.length == 0) {
          // We have no expressions so create an empty expression that spans the entire input length
          const artificialStart = this.offset;
          const artificialEnd = this.offset + this.inputLength;
          return new EmptyExpr(
              this.span(artificialStart, artificialEnd),
              this.sourceSpan(artificialStart, artificialEnd));
        }
        if (exprs.length == 1) return exprs[0];
        return new Chain(this.span(start), this.sourceSpan(start), exprs);*/
    }

    Ast::AST parsePrefix() {
        if (next().type == Lexer::TokenType::Operator) {
            const int start = inputIndex();
            const char& _operatorChar = next().strValue.at(0);
            //Ast::AST result;
            switch (_operatorChar) {
            case '+':
                advance();
                return Unary.createPlus(span(start), sourceSpan(start), parsePrefix());
            case '-':
                advance();
                return Unary.createMinus(span(start), sourceSpan(start), parsePrefix());
            case '!':
                advance();
                return new PrefixNot(span(start), sourceSpan(start), parsePrefix());
            }
        }

        return parseCallChain();


    };



};


// UnquotedChars

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

// Parser region

Ast::ASTWithSource Parser::Parser::parseAction(std::string input, std::string location, int absoluteOffset, InterpolationConfig& interpolationConfig = DEFAULT_INTERPOLATION_CONFIG) {
    _checkNoInterpolation(input, location, interpolationConfig);
    std::vector<Lexer::Token> tokens = lexer.tokenize(_stripComments(input));
    let flags = ParseFlags.Action;
    if (isAssignmentEvent) {
        flags |= ParseFlags.AssignmentEvent;
    }
    _ParseAST parseAst(input, location, absoluteOffset, tokens, sourceToLex.size(), true, errors, input.size() - sourceToLex.size())
        ast = parseAst.parseChain();
    return Ast::ASTWithSource(ast, input, location, absoluteOffset, errors);
}



std::string Parser::Parser::_stripComments(std::string input) {
    int i = _commentStart(input);
    return i != -1 ? trim(input.substr(0, i)) : input;
}

int Parser::Parser::_commentStart(std::string input) {
    int outerQuote = -1;
    for (auto i = 0; i < input.size() - 1; ++i) {
        char currChar = Lexer::charCodeAt(input, i);
        char nextChar = Lexer::charCodeAt(input, i + 1);

        if (currChar == Chars::$SLASH && nextChar == Chars::$SLASH && outerQuote == -1) return i;

        if (outerQuote == currChar) {
            outerQuote = -1;
        } else if (outerQuote == -1 && Chars::isQuote(currChar)) {
            outerQuote = currChar;
        }
    }

    return -1;
}

void Parser::Parser::_checkNoInterpolation(std::string input, std::string location, InterpolationConfig interpolationConfig) {
    int startIndex = -1;
    int endIndex = -1;

    UnquotedChars unquotedChars{ input };

    while (auto charIndex = unquotedChars.next()) {
        if (startIndex == = -1) {
            if (input.startsWith(start)) {
                startIndex = charIndex;
            }
        }
        else {
            endIndex = this._getInterpolationEndIndex(input, end, charIndex);
            if (endIndex > -1) {
                break;
            }
        }
    }

    if (startIndex > -1 && endIndex > -1) {
        this._reportError(
            `Got interpolation(${ start }${ end }) where expression was expected`, input,
            `at column ${ startIndex } in`, location);
    }
}


std::string trim(const std::string val) {
    std::string result = val;
    ltrim(result);
    rtrim(result);
    return result;
}

void ltrim(std::string& val) {
    val.erase(val.begin(), std::find_if(val.begin(), val.end(), [](unsigned char character) { return !std::isspace(character); }));
}

void rtrim(std::string& val) {
    val.erase(std::find_if(val.rbegin(), val.rend(), [](unsigned char character) { return !std::isspace(character); }).base(), val.end());
}