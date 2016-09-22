#include "token_stream.h"
#include "parse_exception.h"

#include <sstream>
#include <iostream>

TokenStream::TokenStream( InputStream inputStream )
    : mInputStream( std::move( inputStream ) ),
      mIndentationStack( {-1} ) {
}

TokenStream::~TokenStream() {}

TokenPtr TokenStream::next() {
    // Ignoring escaped NewLines (\ followed by a newLine)
    if(mInputStream.peek(0) == '\\' &&
	isNewLine( 1 ) ) {
	mInputStream.next();
	newLine();
    }
    if( isComment() ) {
	return comment();
    } else if( isMultiLineComment() ) {
	return multilineComment();
    } else if( isNewLine() ) {
	return newLine();
    } else if( isPreprocessor() ) {
	return preprocessor();
    } else if( isSeperator() ) {
	return seperator();
    } else if( isIdentifier() ) {
	return identifier();
    } else if( isString() ) {
	return string();
    } else if( isLongString() ) {
	return longString();
    } else if( isOperator() ) {
	return Operator();
    } else if( isSpecialOperator() ) {
	return specialOperator();
    } else if( isColor() ) {
	return color();
    } else if( isNumber() ) {
	return number();
    } else if( isIcon() ) {
	return icon();
    } else if( isWhitespace() ) {
	return whitespace();
    } else {
	char nextChar = mInputStream.peek();
	std::stringstream ss;
	ss << (int)nextChar;
	ss << "[" << nextChar << "]";
	throwError("Unknown char encountered: " + ss.str() );
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
    return  mInputStream.peek(0) == '/' &&
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
    return Token::create( Token::Comment, value );
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
    return Token::create( Token::Comment, value );
}

bool TokenStream::isPreprocessor() {
    return mInputStream.peek() == '#';
}

TokenPtr TokenStream::preprocessor() {
    mInputStream.next();
    return Token::create( Token::Preprocessor, identifier()->getContent() );
}

bool TokenStream::isSeperator() {
    return  (mInputStream.peek(0) == '/' &&
	     mInputStream.peek(1) != '/' &&
	     mInputStream.peek(1) != '*') ||
	     mInputStream.peek(0) == ',' ||
	     mInputStream.peek(0) == '.' ||
	     mInputStream.peek(0) == ':' ||
	     mInputStream.peek(0) == ';';
}

TokenPtr TokenStream::seperator() {
    return Token::create( Token::Seperator, mInputStream.next() );
}

//-----------------------------------------------------------
//----------------- Old -------------------------------------
//-----------------------------------------------------------

bool TokenStream::isNewLine( int offset ) {
    return !mInputStream.eof() &&
	(mInputStream.peek( offset ) == '\r' ||
	 mInputStream.peek( offset ) == '\n');
}

TokenPtr TokenStream::newLine() {
    while( isNewLine() ) {
	mInputStream.next();
    }
    return Token::create( Token::NewLine );
}

bool TokenStream::isIdentifier( bool insideTernary ) {
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
    return Token::create( Token::Identifier, value );
}

bool TokenStream::isClosingSpecialOperator() {
    char c = mInputStream.peek();
    return  c == ')' ||
	    c == ']' ||
	    c == '}';
}

bool TokenStream::isOpeningSpecialOperator() {
    char c = mInputStream.peek();
    return  c == '(' ||
	    c == '[' ||
	    c == '{';
}

bool TokenStream::isSpecialOperator() {
    return  isClosingSpecialOperator() ||
	    isOpeningSpecialOperator();
    
}

TokenPtr TokenStream::specialOperator() {
    return Token::create( Token::SpecialOperator,
			  mInputStream.next() );
}

bool TokenStream::isOperator() {
    if( mInputStream.eof() ) {
	return false;;
    }
    char c = mInputStream.peek(0);
    char c2 = ' ';
    if( !mInputStream.eof(1) ) {
	c2 = mInputStream.peek(1);
    }
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
	    c == '~' ||
	    c == '%' ||
	    c == '?' ||
	    c == '^';
}

TokenPtr TokenStream::Operator() {
    std::string value;
    value += mInputStream.next();
    if( !mInputStream.eof() &&
	isOperator() ) {
	value += mInputStream.next();
	if( isOperator() ) {
	    value += mInputStream.next();
	}
    }
    return Token::create( Token::Operator, value );
}

bool TokenStream::isWhitespace( uint32_t offset ) {
    return  !mInputStream.eof() &&
	(mInputStream.peek( offset ) == ' ' ||
	 mInputStream.peek( offset ) == 9);
}

TokenPtr TokenStream::whitespace() {
    std::string value = "";
    while(isWhitespace()) {
	value += mInputStream.next();
    }
    return Token::create( Token::Whitespace, value );
}

bool TokenStream::isString() {
    return mInputStream.peek() == '"';
}

TokenPtr TokenStream::string() {
    mInputStream.next();
    std::string value;
    int insideSquareBrackets = 0;
    while( !isString() ||
	insideSquareBrackets > 0 ) {
	char c = mInputStream.peek( 0 );
	if( isNewLine() ) {
	    throwError("Encountered newline inside string");
	}
	if( c == '[') {
	    insideSquareBrackets++;
	}
	if( c == ']') {
	    insideSquareBrackets--;
	}
	if( c == '\\') {
	    char c2 = mInputStream.peek( 1 );
	    if( c2 == '"' ||
		c2 == '\'' ||
		c2 == '\\' ||
		c2 == '[' ||
		c2 == ']') {
		mInputStream.next();
	    }
	    // Skip the escaped NewLine
	    if( isNewLine( 1 ) ) {
		mInputStream.next();
		newLine();
	    }
	}
	value += mInputStream.next();

    }
    mInputStream.next();
    return Token::create( Token::String, value );
}

bool TokenStream::isLongString() {
    return  mInputStream.peek(0) == '{' &&
	    mInputStream.peek(1) == '"';
}

TokenPtr TokenStream::longString() {
    mInputStream.next();
    mInputStream.next();
    int insideSquareBrackets = 0;
    std::string value;
    while( (mInputStream.peek(0) != '"' ||
	    mInputStream.peek(1) != '}') ||
	    insideSquareBrackets > 0 ) {
	char c = mInputStream.peek( 0 );
	if( c == '[') {
	    insideSquareBrackets++;
	}
	if( c == ']') {
	    insideSquareBrackets--;
	}
	value += mInputStream.next();
    }
    mInputStream.next();
    mInputStream.next();
    return Token::create( Token::String, value );
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
    while( !mInputStream.eof() &&
	   (isNumber() ||
	    mInputStream.peek() == '.' )) {
	value += mInputStream.next();
    }
    return Token::create( Token::Number, value );
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
    return Token::create( Token::Color, value );
}

bool TokenStream::isIcon() {
    return mInputStream.peek() == '\'';
}

TokenPtr TokenStream::icon() {
    mInputStream.next();
    std::string value = "";
    while( !isIcon() ) {
	char c = mInputStream.peek( 0 );
	if( c == '\\') {
	    char c2 = mInputStream.peek( 1 );
	    if( c2 == '\'' ) {
		mInputStream.next();
	    }
	}
	value+= mInputStream.next();
    }
    mInputStream.next();
    return Token::create( Token::Icon, value );
}
