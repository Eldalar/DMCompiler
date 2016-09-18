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
	
    virtual std::string str() override;

    static std::unique_ptr<Token> create( std::string content );
private:
    //! Copy-Constructor (delete)
    Atom( const Atom & ) = delete;
    //! Asignment-Operator (delete)
    Atom &operator=( const Atom & ) = delete;

    std::string mContent;
};

#endif // ATOM_H
