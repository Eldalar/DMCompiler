#include "input_stream.h"
#include "parse_exception.h"

#include <tuple>
#include <iostream>

InputStream::InputStream( std::string fileName )
    : mFileName( fileName ),
      mLine(1),
      mColumn(1),
      mPosition(0) {
    std::ifstream is (fileName,
		      std::ifstream::binary);
    if( is.is_open() ) {
	is.seekg (0, is.end);
	mLength = is.tellg();
	is.seekg (0, is.beg);
	mBuffer = new char[ mLength ];
	is.read(mBuffer, mLength);
	is.close();
    } else {
	throw ParseException("Unable to open file",
			     mFileName,
			     mLine,
			     mColumn );
    }
}

InputStream::InputStream( InputStream && rhs )
    : mFileName( rhs.mFileName ),
      mLine( rhs.mLine ),
      mColumn( rhs.mColumn),
      mPosition( rhs.mPosition ),
      mBuffer( rhs.mBuffer ),
      mLength( rhs.mLength ) {
    rhs.mBuffer = nullptr;
}

InputStream::~InputStream() {
    if( mBuffer ) {
	delete[] mBuffer;
    }
}

char InputStream::next() {
    if( eof() ) {
	throw ParseException("Unexpected eof during next",
			     mFileName,
			     mLine,
			     mColumn );
    }
    char c = mBuffer[mPosition];
    mColumn++;
    if( c == '\n' ) {
	mColumn = 1;
	mLine++;
    }

    mPosition++;
    return c;
}

char InputStream::peek( size_t offset ) {
    if( eof() ) {
	throw ParseException("Unexpected eof during peek",
			     mFileName,
			     mLine,
			     mColumn );
    }
    return mBuffer[ mPosition + offset ];
}

bool InputStream::eof( size_t offset ) const {
    return (mPosition + offset) >= mLength;
}

std::string InputStream::getFileName() const {
    return mFileName;
}

uint32_t InputStream::getColumn() const {
    return mColumn;
}

uint32_t InputStream::getLine() const {
    return mLine;
}

