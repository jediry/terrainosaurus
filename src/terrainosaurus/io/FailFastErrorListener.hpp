/**
 * \file    FailFastErrorListener.hpp
 *
 * \author  Ryan L. Saunders
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file defines the FailFastErrorListener, an implementation
 *      of antlr4::ANTLRErrorListener that thows an
 *      inca::io::FileFormatException in response to syntax errors.
 */

#include <inca/io/FileExceptions.hpp>
#include <BaseErrorListener.h>

namespace terrainosaurus {

/**
 * The FailFastErrorListener class extends antlr4::BaseErrorListener to throw inca::io::FileFormatException in response
 * to syntax errors found during parsing, thus immediately canceling the parse. The exception contains the location
 * and description of the problem.
 */
class FailFastErrorListener final : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                     size_t charPositionInLine, const std::string &msg, std::exception_ptr e) override {
        auto sourceName = recognizer->getInputStream()->getSourceName();
        throw inca::io::FileFormatException(sourceName, line, charPositionInLine, msg);
    }
};

}  // namespace terrainosaurus
