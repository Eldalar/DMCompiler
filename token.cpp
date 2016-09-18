#include "token.h"
#include <sstream>
#include <memory>

Token::Token( Type type,
	      Content content )
    : mType( type ) {
    mContent.push_back( std::move( content ) );
}

Token::Token( Type type,
	      std::vector<std::unique_ptr<Token>> content )
    : mType( type ),
      mContent( std::move( content ) ) {}

Token::~Token() {}

Token::Type Token::getType() {
    return mType;
}

TokenPtr& Token::getContent( uint32_t index ) {
    return mContent[index];
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
    }
    return "Unknown";
}

#include <iostream>
std::string Token::str() {
    std::stringstream ss;
    ss << "{ Type: " << ToString( mType ) << " ";
    ss << " Content: ";
    for( std::unique_ptr<Token>& content : mContent ) {
	ss << content->str();
	ss << " ";
	}
    ss << "}";
    return ss.str();
}

std::unique_ptr<Token> Token::create( Type type,
				      std::vector<Content> content ) {
    return std::unique_ptr<Token>( new Token( type, std::move(content) ));
}

std::unique_ptr<Token> Token::create( Type type,
				      Content content ) {
    return std::unique_ptr<Token>( new Token( type, std::move(content) ));
}
