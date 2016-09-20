#include "token_stream.h"
#include "parse_exception.h"

#include "atom.h"

#include <sstream>
#include <iostream>

TokenStream::TokenStream( InputStream inputStream )
    : mInputStream( std::move( inputStream ) ),
      mIndentationStack( {0} ) {
}

TokenStream::~TokenStream() {}

TokenPtr TokenStream::next() {
    bool repeat = false;
    while(!mInputStream.eof()) {
	char nextchar = mInputStream.peek();
	repeat = false;
	if( isComment() ) {
	    return comment();
	} else 
	if( isMultiLineComment() ) {
	    return multilineComment();
	} else if( isExpression() ) {
	    return expression();
	} else if( isNewLine() ) {
	    newLine();
	} else if( isPreprocessor() ) {
	    return preprocessor();
	} else {
	    std::stringstream ss;
	    ss << (int)nextchar;
	    ss << "[" << nextchar << "]";
	    throwError("Unknown char encountered: " + ss.str() );
	}
    }
    return nullptr;
}

void TokenStream::throwError( std::string error ) {
    std::string snippet;
    for( size_t i = 0; i < 20; ++ i ) {
	snippet += mInputStream.next();
    }
    throw ParseException(error + "\n"+ snippet,
			 mInputStream.getFileName(),
			 mInputStream.getLine(),
			 mInputStream.getColumn() );
}
 
bool TokenStream::eof() const {
    return mInputStream.eof();
}

bool TokenStream::isComment() {
    return  mInputStream.peek() == '/' &&
	    mInputStream.peek(1) == '/';
}

TokenPtr TokenStream::comment() {
    if( mInputStream.next() != '/' ) {
	throwError("Kein / vorgefunden in comment" );
    }
    if( mInputStream.next() != '/' ) {
	throwError("Kein zweites / vorgefunden in comment" );
    }
    std::string value = "";
    while( !mInputStream.eof() &&
	   !isNewLine() ) {
	value += mInputStream.next();
    }
    if( !mInputStream.eof() ) {
	newLine();
    }
    return Token::create( Token::Comment, Atom::create(value) );
}

bool TokenStream::isMultiLineComment() {
    return  mInputStream.peek(0) == '/' &&
	    mInputStream.peek(1) == '*';
}

TokenPtr TokenStream::multilineComment() {
    if( mInputStream.next() != '/' ) {
	throwError("Kein / vorgefunden in multiline comment" );
    }
    if( mInputStream.next() != '*' ) {
	throwError("Kein * vorgefunden in multiline comment" );
    }
    std::string value = "";
    while( !mInputStream.eof() &&
	   (mInputStream.peek(0) != '*' ||
	    mInputStream.peek(1) != '/') ) {
	value += mInputStream.next();
    }
    mInputStream.next();
    mInputStream.next();
    return Token::create( Token::Comment, Atom::create(value) );
}

bool TokenStream::isNewLine() {
    return !mInputStream.eof() &&
	    mInputStream.peek() == '\r';
}

void TokenStream::newLine() {
    if( mInputStream.peek() == '\r' ) {
	mInputStream.next();
	if( mInputStream.peek() == '\n' ) {
	    mInputStream.next();
	}
    } else {
	throwError( "Not really a newLine" );
    }
}

bool TokenStream::isExpression() {
    return  isIdentifier() ||
	    isQuickExpressionStart();
}

TokenPtr TokenStream::expression( bool insideTernary ) {
    std::vector<TokenPtr> expressionParts;
    while( !mInputStream.eof() &&
	   !isClosingSpecialOperator() &&
	   !(insideTernary &&
	     mInputStream.peek() == ':' ) ) {
	if( isQuickExpressionStart() ) {
	    quickExpressionStart();
	} else if( isIdentifier() ) {
	    expressionParts.emplace_back( identifier() );
	} else if( isSpecialOperator() ) {
	    expressionParts.emplace_back( specialOperator() );
	    expressionParts.emplace_back( expression() );
	    expressionParts.emplace_back( specialOperator() );
	} else if( mInputStream.peek() == ',' ) {
	    mInputStream.next();
	} else if( isOperator() ) {
	    expressionParts.emplace_back( Operator() );
	} else if( isTernaryOperator() ) {
	    expressionParts.emplace_back( ternaryOperator() );
	} else if( isWhitespace() ) {
	    skipWhitespace();
	} else if( isString() ) {
	    expressionParts.emplace_back( string() );
	} else if( isLongString() ) {
	    expressionParts.emplace_back( longString() );
	} else if( isNumber() ) {
	    expressionParts.emplace_back( number() );
	} else if( isColor() ) {
	    expressionParts.emplace_back( color() );
	} else if( isComment() ) {
	    expressionParts.emplace_back( comment() );
	} else if( isMultiLineComment() ) {
	    expressionParts.emplace_back( multilineComment() );
	} else if( mInputStream.peek(0) == '\\' &&
		   mInputStream.peek(1) == '\r' ) {
	    // Simple removing escaped newlines
	    mInputStream.next();
	    newLine();
	} else if( isNewLine() ) {
	    while( isNewLine() ) {
		// Skip empty lines
		newLine();
	    }
	    if( isWhitespace() ) {
		int indentation = judgeWhitespace();
		if( indentation <= mIndentationStack.top() ) {
		    break;
		} else {
		    mIndentationStack.push( indentation );
		    expressionParts.emplace_back( expression() );
		    mIndentationStack.pop();
		}
	    } else {
		break;
	    }
	} else {
	    throwError( "Unable to interpret Expression" );
	}
    }
    return Token::create( Token::Expression, std::move(expressionParts) );
}

bool TokenStream::isQuickExpressionStart() {
    return  mInputStream.peek() == '/' &&
	(mInputStream.peek(1) != '/' &&
	 mInputStream.peek(1) != '*' ) ;
}

void TokenStream::quickExpressionStart() {
    mInputStream.next();
}

bool TokenStream::isIdentifier() {
    char c = mInputStream.peek();
    return (c >= 'a' &&
	    c <= 'z') ||
	   (c >= 'A' &&
	    c <= 'Z') ||
	    c == '.' ||
	    c == '_';
}

TokenPtr TokenStream::identifier() {
    std::string value;
    while( !mInputStream.eof() &&
	   (isIdentifier() ||
	    isNumber() )) {
	value += mInputStream.next();
    }
    if( value.size() == 0 ) {
	throwError( "Unable to read the identifier" );
    }
    return Token::create( Token::Identifier, Atom::create(value) );
}

bool TokenStream::isClosingSpecialOperator() {
    char c = mInputStream.peek();
    return  c == ')' ||
	    c == ']';
}

bool TokenStream::isOpeningSpecialOperator() {
    char c = mInputStream.peek();
    return  c == '(' ||
	    c == '[';
}

bool TokenStream::isSpecialOperator() {
    return  isClosingSpecialOperator() ||
	    isOpeningSpecialOperator();
    
}

TokenPtr TokenStream::specialOperator() {
    return Token::create( Token::SpecialOperator,
			  Atom::create( mInputStream.next() ) );
}

bool TokenStream::isTernaryOperator() {
    return mInputStream.peek() == '?';
}

TokenPtr TokenStream::ternaryOperator() {
    std::vector<TokenPtr> tokens;
    mInputStream.next();
    tokens.emplace_back( expression( true ) );
    mInputStream.next();
    tokens.emplace_back( expression( ) );
    return Token::create( Token::TernaryOperator, std::move( tokens ) ) ;
}

bool TokenStream::isOperator() {
    char c = mInputStream.peek();
    char c2 = mInputStream.peek();
    return  c == '+' ||
	    c == '-' ||
	    c == '*' ||
	    (c == '/' && c2 != '/' && c2 != '*')||
	    c == '=' ||
	    c == '|' ||
	    c == '&' ||
	    c == '>' ||
	    c == '<' ||
	    c == '!' ||
	    c == '%';
}

TokenPtr TokenStream::Operator() {
    std::string value;
    value += mInputStream.next();
    if( isOperator() ) {
	value += mInputStream.next();
	if( isOperator() ) {
	    value += mInputStream.next();
	}
    }
    return Token::create( Token::Operator, Atom::create( value ) );
}

bool TokenStream::isWhitespace( uint32_t offset ) {
    return  !mInputStream.eof() &&
	(mInputStream.peek( offset ) == ' ' ||
	 mInputStream.peek( offset ) == 9);
}

uint32_t TokenStream::judgeWhitespace() {
    uint32_t offset = 0;
    while(isWhitespace(offset)) {
	offset++;
    }
    return offset;
}

void TokenStream::skipWhitespace() {
    while( isWhitespace() ) {
	mInputStream.next();
    }
}

bool TokenStream::isPreprocessor() {
    return mInputStream.peek() == '#';
}

TokenPtr TokenStream::preprocessor() {
    mInputStream.next();
    return Token::create( Token::Preprocessor, expression() );
}

bool TokenStream::isString() {
    return mInputStream.peek() == '"';
}

TokenPtr TokenStream::string() {
    mInputStream.next();
    std::string value;
    while( !isString() ) {
	char c = mInputStream.peek( 0 );
	if( c == '\\') {
	    char c2 = mInputStream.peek( 1 );
	    if( c2 == '"' ||
		c2 == '\\') {
		mInputStream.next();
	    }
	}
	value += mInputStream.next();
    }
    mInputStream.next();
    return Token::create( Token::String, Atom::create( value ) );
}

bool TokenStream::isLongString() {
    return  mInputStream.peek(0) == '{' &&
	    mInputStream.peek(1) == '"';
}

TokenPtr TokenStream::longString() {
    mInputStream.next();
    mInputStream.next();
    std::string value;
    while( mInputStream.peek(0) != '"' ||
	   mInputStream.peek(1) != '}' ) {
	value += mInputStream.next();
    }
    mInputStream.next();
    mInputStream.next();
    return Token::create( Token::String, Atom::create( value ) );
}

bool TokenStream::isNumber() {
    char c = mInputStream.peek();
    return  c >= '0' &&
	    c <= '9';
}

bool TokenStream::isHexNumber() {
    char c = mInputStream.peek();
    return  (c >= '0' &&
	     c <= '9') ||
	    (c >= 'a' &&
	     c <= 'f');
}

TokenPtr TokenStream::number() {
    std::string value = "";
    while( isNumber() ||
	   mInputStream.peek() == '.' ) {
	value += mInputStream.next();
    }
    return Token::create( Token::Number, Atom::create( value ) );
}

bool TokenStream::isColor() {
    return mInputStream.peek() == '#';
}

TokenPtr TokenStream::color() {
    std::string value = "";
    while( isColor() ||
	   isHexNumber() ) {
	value+= mInputStream.next();
    }
    return Token::create( Token::Color, Atom::create( value ) );
}

