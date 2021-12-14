#include <algorithm>;
#include <cctype>
#include <map>
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
    std::vector<Lexer::Token> tokens;

    _ParseAST(
        std::string input, std::string location, int absoluteOffset,
        std::vector<Lexer::Token> tokens, int inputLength, bool parseAction,
        std::vector<Ast::ParserError> errors, int offset
    ) : tokens{ tokens }, offset{ offset }, inputLength{ inputLength }, absoluteOffset{ absoluteOffset }{}

    Lexer::Token peek(int offset) {
        int i = index + offset;
        return i < tokens.size() ? tokens[i] : Lexer::_EOF;
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

};

Parser::Parser Parser::Parser::parseAction(std::string input, std::string location, int absoluteOffset, InterpolationConfig& interpolationConfig = DEFAULT_INTERPOLATION_CONFIG) {
    std::string sourceToLex = _stripComments(input);
    std::vector<Lexer::Token> tokens = lexer.tokenize(_stripComments(input));
    _ParseAST parseAst(input, location, absoluteOffset, tokens, sourceToLex.size(), true, errors, input.size() - sourceToLex.size())
    ast = parseAst.parseChain();
    return Ast::ASTWithSource(ast, input, location, absoluteOffset, errors);
}

std::string Parser::Parser::_stripComments(std::string input) {
    const std::optional<int> i = {}; // this._commentStart(input);
    return i.has_value() ? trim(input.substr(0, i.value())) : input;
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