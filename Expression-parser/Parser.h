#pragma once
#include <string>;
#include <vector>;
#include <optional>;
#include "Lexer.h";
#include "Ast.h";
#include "InterpolationConfig.h";

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
        Parser(Lexer::Lexer& lexer) : lexer{ lexer } {}

        // TODO: simpleExpressionChecker = SimpleExpressionChecker;
    public:
        Ast::ASTWithSource parseAction(std::string input, std::string location, int absoluteOffset, InterpolationConfig& interpolationConfig = DEFAULT_INTERPOLATION_CONFIG);
        Ast::ASTWithSource parseBinding(std::string input, std::string location, int absoluteOffset, InterpolationConfig& interpolationConfig = DEFAULT_INTERPOLATION_CONFIG);
        Ast::ASTWithSource parseSimpleBinding(std::string input, std::string location, int absoluteOffset, InterpolationConfig& interpolationConfig = DEFAULT_INTERPOLATION_CONFIG);
        // TODO: fill the rest
    private:
        std::vector<std::string> checkSimpleExpression(Ast::AST& ast);
        void _reportError(std::string message, std::string input, std::string errLocation, std::optional<std::string> ctxLocation);
        Ast::AST _parseBindingAst(std::string input, std::string location, int absoluteOffset, InterpolationConfig interpolationConfig);
        std::optional<Ast::AST> _parseQuote(std::optional<std::string> input, std::string location, int absoluteOffset);
        // TODO: fill the rest
        std::string _stripComments(std::string input)

    };
}