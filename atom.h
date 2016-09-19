#ifndef ATOM_H
#define ATOM_H
#include "token.h"

/**
 * A Token that is representing a single Value
 **/
class Atom
    : public Token
{
public:
    //! Constructor
    Atom( std::string content );
    //! Destructor
    ~Atom();
    //! Move-Constructor (default)
    Atom( Atom && ) = default;
	
    virtual std::string str( int indentation = 0 ) override;

    static std::unique_ptr<Token> create( char content );
    static std::unique_ptr<Token> create( std::string content );

    virtual bool isAtom() const override;
private:
    //! Copy-Constructor (delete)
    Atom( const Atom & ) = delete;
    //! Asignment-Operator (delete)
    Atom &operator=( const Atom & ) = delete;

    std::string mContent;
};

#endif // ATOM_H
