#include "atom.h"

Atom::Atom( std::string content )
    : Token( Token::Atom ),
      mContent( content ) {
}

Atom::~Atom() {
}

std::string Atom::str( int /*indentation*/) {
    return mContent;
}

std::unique_ptr<Token> Atom::create( std::string content ) {
    return std::unique_ptr<Token>( new Atom( content ) );
}

bool Atom::isAtom() const {
    return true;
}
