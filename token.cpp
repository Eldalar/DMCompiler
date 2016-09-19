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

bool Token::isAtom() const {
    return false;
}

void indent( std::stringstream& ss, int indentation ) {
    for( int i = 0;
	 i < indentation;
	 ++i) {
	ss << "  ";
    }
}

std::string Token::str( int indentation ) {
    int next_indentation = indentation + 1;
    if( indentation < 0 ) {
	next_indentation = indentation;
    }
    std::stringstream ss;
    indent(ss, indentation);
    ss << "{" << std::endl;
    indent(ss, next_indentation);
    ss << "Type: " << ToString( mType ) << std::endl;;
    indent(ss, next_indentation);
    ss << "Content: ";
    bool isAtom = !(mContent.size() > 1 ||
		    (mContent.size() > 0 && !mContent[0]->isAtom() ));
    if( !isAtom ) {
	ss  << std::endl;
    }
    for( std::unique_ptr<Token>& content : mContent ) {
	ss << content->str( next_indentation );
    }
    if( isAtom ) {
	ss << std::endl;
    }
    indent(ss, indentation);
    ss << "}" << std::endl;
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
