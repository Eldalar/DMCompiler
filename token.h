#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

/**
 * Models a single Token of the token stream
 **/
class Token
{
public:
    enum Type : uint32_t {
	Comment,
	Preprocessor,
	Identifier,
	Expression,
	Atom,
	Operator,
	SpecialOperator,
	TernaryOperator,
	String,
	File,
	Number,
	Color,
	Icon,
	Seperator,
	NewLine,
	Whitespace
    };
    //! Constructor
    Token( Type type,
	   std::string content );
    //! Constructor
    Token( Type type,
	   char content );
    //! Destructor
    virtual ~Token();
    //! Move-Constructor (default)
    Token( Token && ) = default;

    virtual std::string str();

    static std::unique_ptr<Token> create( Type type,
					  std::string content = "" );
    static std::unique_ptr<Token> create( Type type,
					  char content );

    Type getType();
    std::string getContent();
private:
    //! Copy-Constructor (delete)
    Token( const Token & ) = delete;
    //! Asignment-Operator (delete)
    Token    &operator=( const Token & ) = delete;

    static std::string ToString( Type type );

    Type mType;
    std::string mContent;
};

using TokenPtr = std::unique_ptr<Token>;

#endif // TOKEN_H
