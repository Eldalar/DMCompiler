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
    using Content = std::unique_ptr<Token>;
public:
    enum Type : uint32_t {
	Comment,
	Preprocessor,
	Identifier,
	Expression,
	Atom,
	Operator,
	String,
	File,
	Number
    };
    //! Constructor
    Token( Type type,
	   Content content );
    //! Constructor
    Token( Type type,
	   std::vector<Content> content = {} );
    //! Destructor
    virtual ~Token();
    //! Move-Constructor (default)
    Token( Token && ) = default;

    virtual std::string str( int indentation = 0 );

    static std::unique_ptr<Token> create( Type type,
					  std::vector<Content> content = {} );
    static std::unique_ptr<Token> create( Type type,
					  Content content );

    Type getType();
    std::unique_ptr<Token>& getContent( uint32_t index );

    virtual bool isAtom() const;
private:
    //! Copy-Constructor (delete)
    Token( const Token & ) = delete;
    //! Asignment-Operator (delete)
    Token    &operator=( const Token & ) = delete;

    static std::string ToString( Type type );

    Type mType;
    std::vector<std::unique_ptr<Token>> mContent;
};

using TokenPtr = std::unique_ptr<Token>;

#endif // TOKEN_H
