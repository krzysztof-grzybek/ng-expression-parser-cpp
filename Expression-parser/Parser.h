#pragma once
#include <string>;
#include <vector>;
#include <optional>;
#include "Lexer.h";
#include "Ast.h";

namespace Parser {
    struct InterpolationPiece {
        std::string text;
        int start;
        int end;
    };

    struct SplitInterpolation {
        std::vector<InterpolationPiece> string;
        std::vector<InterpolationPiece> expressions;
        std::vector<int> offsets;
    };

    struct TemplateBindingParseResult {
        std::vector<Ast::TemplateBinding> templateBindings;
        std::vector<std::string> warnings;
        std::vector<Ast::ParserError> errors;
    };

    class Parser {
    private:
        std::vector<Ast::ParserError> errors;
        Lexer::Lexer lexer;
        Parser(Lexer::Lexer lexer) : lexer{ lexer } {}

        // TODO: simpleExpressionChecker = SimpleExpressionChecker;
    public:
        Ast::ASTWithSource parseAction(std::string input, std::string location, int absoluteOffset ) // TODO: last arg: interpolationConfig: InterpolationConfig = DEFAULT_INTERPOLATION_CONFIG
    };
}