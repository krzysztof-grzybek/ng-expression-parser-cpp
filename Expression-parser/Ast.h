#pragma once
#include <variant>;
#include <string>;
#include <optional>;
using namespace std::literals::string_literals;

namespace Ast {
    struct ParserError {
        std::string message;
        std::string input;
        std::string errLocation;
        std::string ctxLocation; // TODO: it was "optional any" in ts

        ParserError(std::string message, std::string input, std::string errLocation, std::string ctxLocation) : input{ input }, errLocation{ errLocation }, ctxLocation{ ctxLocation } {
            message = "Parser Error : "s + message + errLocation + "[" + input + "] in " + ctxLocation;
        }
    };

    struct ParseSpan {
        int start;
        int end;
        ParseSpan(int start, int end) : start{ start }, end{ end } {}
        AbsoluteSourceSpan toAbsolute(int absoluteOffset) {
            return AbsoluteSourceSpan(absoluteOffset + start, absoluteOffset + end);
        }
    };

    class AstVisitor {}; // TODO: implement

    class AST {
        ParseSpan span;
        AbsoluteSourceSpan sourceSpan;
    public:
        AST(ParseSpan span, AbsoluteSourceSpan sourceSpan) : span{ span }, sourceSpan{ sourceSpan } {}

        virtual void visit(AstVisitor visitor, std::string context) = 0; // context was optional any, return type was any

        virtual std::string toString() {
            return "AST";
        }
    };
    



    // TODO: random order from here, fix it
    struct  AbsoluteSourceSpan {
        const int start;
        const int end;
        AbsoluteSourceSpan(int start, int end) : start{ start }, end{ end }{}
    };

    class ASTWithSource : AST {
        AST* ast;
        std::optional<std::string> source;
        std::string location;
        int absoluteOffset;
        std::vector<ParserError> errors;
        ASTWithSource(AST* ast, std::optional<std::string> source, std::string location, int absoluteOffset, std::vector< ParserError> errors) :
            AST{
                ParseSpan{ 0, source.has_value() ? source.value().size() : 0},
                AbsoluteSourceSpan{ absoluteOffset, source.has_value() ? absoluteOffset + source.value().size() : absoluteOffset }
            },
            ast{ ast }, source{ source }, location{ location }, absoluteOffset{ absoluteOffset }, errors{ errors } {}

        void visit(AstVisitor visitor, std::string context) override { // context was any optional, default to null
            // TODO: implement once AstVisitor is ready
            /*if (visitor.visitASTWithSource) {
                return visitor.visitASTWithSource(this, context);
            }

            return ast.visit(visitor, context);*/
        }

        std::string toString() override {
            return source.value_or("") + " in " + location;
        }
    };

    struct TemplateBindingIdentifier {
        std::string source;
        AbsoluteSourceSpan span;
    };

    struct VariableBinding {
        AbsoluteSourceSpan sourceSpan;
        TemplateBindingIdentifier key;
        std::optional<TemplateBindingIdentifier> value;
    };
    struct ExpressionBinding {};

    typedef std::variant< VariableBinding, ExpressionBinding> TemplateBinding; \
};