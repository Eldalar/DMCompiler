#include "file_reader.h"
#include "input_stream.h"
#include "token_stream.h"
#include "parse_exception.h"

#include <iostream>

FileReader::FileReader( std::string folder, std::string mainFile )
    : mFolder( folder ) {
    mFileNames.push_back( mainFile );
}

FileReader::~FileReader() {
}

bool FileReader::readFile( std::string file ) {
    try {
	InputStream inputStream( mFolder+file );
	TokenStream tokenStream( std::move( inputStream ) );
	std::vector<TokenPtr> subtokens;
	while( !tokenStream.eof() ) {
	    TokenPtr token = tokenStream.next();
	    if( token) {
		if( token->getType() == Token::Preprocessor ) {
		    TokenPtr& subtoken = token->getContent(0);
		    if( subtoken->getContent( 0 )->getContent(0)->str() == "include" ) {
			std::string includedFile = subtoken->getContent(1)->getContent(0)->str();
			if( mFileTokens.find( includedFile ) == mFileTokens.end() ) {
			    mFileNames.push_back( includedFile );
			}
		    }
		}
		//std::cout << token->str() << std::endl;
		subtokens.emplace_back( std::move( token ) );
	    }
	}
	mFileTokens.emplace( file,
			     Token::create( Token::File, std::move( subtokens ) ) );

    }
    catch( ParseException& parseException ) {
	std::cout << parseException.toError() << std::endl;
	return false;
    }
    return true;
}

void FileReader::run() {
    uint64_t success = 0;
    uint64_t total = 0;
    while(mFileNames.size() > 0 ) {
	std::string fileName = mFileNames.back();
	mFileNames.pop_back();
	if( readFile( fileName ) ) {
	    success++;
	}
	total++;
    }
    std::cout << success << "/" << total << "(" << 100 * success/total << "%) suceeded!" << std::endl;
}
