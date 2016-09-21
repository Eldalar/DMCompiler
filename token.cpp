#include "token.h"
#include <sstream>
#include <memory>

Token::Token( Type type,
	      std::string content )
    : mType( type ),
      mContent( content ) {
}

Token::Token( Type type,
	      char content )
    : mType( type ) {
    mContent = content;
}

Token::~Token() {}

Token::Type Token::getType() {
    return mType;
}

std::string Token::getContent() {
    return mContent;
}

std::string Token::ToString( Type type ) {
    switch( type ) {
    case Token::Comment: {
	return "Comment";
    }
    case Token::Preprocessor: {
	return "Preprocessor";
    }
    case Token::Atom: {
	return "Atom";
    }
    case Token::Expression: {
	return "Expression";
    }
    case Token::Identifier: {
	return "Identifier";
    }
    case Token::Operator: {
	return "Operator";
    }
    case Token::String: {
	return "String";
    }
    case Token::File: {
	return "File";
    }
    case Token::Number: {
	return "Number";
    }
    case Token::Color: {
	return "Color";
    }
    case Token::Icon: {
	return "Icon";
    }
    case Token::Seperator: {
	return "Seperator";
    }
    case Token::NewLine: {
	return "Whitespace";
    }
    case Token::Whitespace: {
	return "Whitespace";
    }
    }
    return "Unknown";
}

void indent( std::stringstream& ss, int indentation ) {
    for( int i = 0;
	 i < indentation;
	 ++i) {
	ss << "  ";
    }
}

std::string Token::str() {
    std::stringstream ss;
    ss << "{";
    ss << " Type: " << ToString( mType );
    ss << " Content: " << mContent;
    ss << " }";
    return ss.str();
}

std::unique_ptr<Token> Token::create( Type type,
				      std::string content ) {
    return std::unique_ptr<Token>( new Token( type, content ));
}

std::unique_ptr<Token> Token::create( Type type,
				      char content ) {
    return std::unique_ptr<Token>( new Token( type, content ));
}
