#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H
#include "token.h"
#include "input_stream.h"

#include <stack>

/**
 * Returns a stream of tokens depending on the given InputStream
 **/
class TokenStream
{
public:
    //! Constructor
    TokenStream( InputStream inputStream );
    //! Destructor
    ~TokenStream();
    //! Move-Constructor (default)
    TokenStream( TokenStream && ) = default;

    TokenPtr next();
    bool eof() const;
private:
    //! Determines if the next token is a comment
    bool isComment();
    //! Parses a comment token
    TokenPtr comment();
    //! Determines if the next token is a multiline comment
    bool isMultiLineComment();
    //! Parses a multiline comment token
    TokenPtr multilineComment();
    //! Determines if the next token is a newline
    bool isNewLine();
    //! Parses a newLine
    void newLine();
    //! Determines if the next token is an expression
    bool isExpression();
    //! Parses an expression
    TokenPtr expression( bool insideTernary = false );
    //! Determines if the next token is an QuickExpressionStart
    bool isQuickExpressionStart();
    //! Parses an QuickExpressionStart
    void quickExpressionStart();
    //! Determines if the next token is an Operator
    bool isOperator();
    //! Parses an Operator
    TokenPtr Operator();
    //! Determines if the next token is a Closing Special Operator
    bool isClosingSpecialOperator();
    //! Determines if the next token is an Opening Special Operator
    bool isOpeningSpecialOperator();
    //! Determines if the next token is a Special Operator
    bool isSpecialOperator();
    //! Parses a Special Operator
    TokenPtr specialOperator();
    //! Determines if the next token is a Ternary Operator
    bool isTernaryOperator();
    //! Parses a Ternary Operator
    TokenPtr ternaryOperator();
    //! Determines if the next token is Whitespace
    bool isWhitespace( uint32_t offset = 0 );
    //! Determines the length of the following Whitespace token
    uint32_t judgeWhitespace();
    //! Skips the Whitespace
    void skipWhitespace();
    //! Determines if the next token is an identifier
    bool isIdentifier();
    //! Parses an identifier
    TokenPtr identifier();
    //! Determines if the next token is Preprocessor Instruction
    bool isPreprocessor();
    //! Parses the Preprocessor Instruction
    TokenPtr preprocessor();
    //! Determines if the next token is a string
    bool isString();
    //! Parses the string
    TokenPtr string();
    //! Determines if the next token is a (long) string
    bool isLongString();
    //! Parses the (long) string
    TokenPtr longString();
    //! Determines if the next token is a number
    bool isNumber();
    //! Parses the number
    TokenPtr number();
    //! Determines if the next token is a color
    bool isColor();
    //! Parses the color
    TokenPtr color();
    //! Throws a ParseException
    void throwError(std::string error );

    //! Copy-Constructor (delete)
    TokenStream( const TokenStream & ) = delete;
    //! Asignment-Operator (delete)
    TokenStream &operator=( const TokenStream & ) = delete;

    InputStream mInputStream;
    std::stack<uint32_t> mIndentationStack;
};

#endif // TOKEN_STREAM_H
