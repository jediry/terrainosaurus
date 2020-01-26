/**
 * \file    PrimitiveBaseListener.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file provides the TPrimitiveBaseListener class template,
 *      for simplifying writing Listeners for grammars derived from
 *      Primitives.g4.
 */

#include <antlr4-runtime.h>

namespace terrainosaurus {

template <typename ListenerInterface, typename ParserType>
class TPrimitivesBaseListener : public ListenerInterface {
protected:
    TPrimitivesBaseListener() { }

public:
    void enterBlankLine(typename ParserType::BlankLineContext * /*ctx*/) override { }
    void exitBlankLine(typename ParserType::BlankLineContext * /*ctx*/) override { }

    void enterPath(typename ParserType::PathContext * /*ctx*/) override { }
    void exitPath(typename ParserType::PathContext * /*ctx*/) override { }

    void enterString(typename ParserType::StringContext * /*ctx*/) override { }
    void exitString(typename ParserType::StringContext * /*ctx*/) override { }

    void enterFraction(typename ParserType::FractionContext * /*ctx*/) override { }
    void exitFraction(typename ParserType::FractionContext * /*ctx*/) override { }

    void enterScalar(typename ParserType::ScalarContext * /*ctx*/) override { }
    void exitScalar(typename ParserType::ScalarContext * /*ctx*/) override { }

    void enterNFraction(typename ParserType::NFractionContext * /*ctx*/) override { }
    void exitNFraction(typename ParserType::NFractionContext * /*ctx*/) override { }

    void enterColor(typename ParserType::ColorContext * /*ctx*/) override { }
    void exitColor(typename ParserType::ColorContext * /*ctx*/) override { }

    void enterInteger(typename ParserType::IntegerContext * /*ctx*/) override { }
    void exitInteger(typename ParserType::IntegerContext * /*ctx*/) override { }

    void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
    void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
    void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
    void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }
};

}  // namespace terrainosaurus
