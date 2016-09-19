#include "file_reader.h"
#include "input_stream.h"
#include "token_stream.h"
#include "parse_exception.h"

#include <iostream>
#include <sstream>

FileReader::FileReader( std::string folder, std::string mainFile )
    : mFolder( folder ) {
    mFileNames.push_back( std::make_pair( mainFile, "" ) );
}

FileReader::~FileReader() {
}

bool exists( std::string file ) {
    std::ifstream is (file,
		      std::ifstream::binary);
    return is.is_open();
}

bool FileReader::readFile( std::string file,
			   std::string directory ) {
    std::stringstream failureLog;
    std::string newDirectory = file.substr(0, file.find_last_of('\\')+1 );
    std::string fullPath = mFolder + file;
    if( !exists( fullPath ) ) {
	fullPath = mFolder + directory + file;
	std::string newPath = directory + file;
	newDirectory = newPath.substr(0, newPath.find_last_of('\\')+1 );
    }
    try {
	InputStream inputStream( fullPath );
	TokenStream tokenStream( std::move( inputStream ) );
	std::vector<TokenPtr> subtokens;
	while( !tokenStream.eof() ) {
	    TokenPtr token = tokenStream.next();
	    if( token) {
		if( token->getType() == Token::Preprocessor ) {
		    TokenPtr& subtoken = token->getContent(0);
		    if( subtoken->getContent( 0 )->getContent(0)->str() == "include" ) {
			std::string includedFile = subtoken->getContent(1)->getContent(0)->str();
			std::string extension = includedFile.substr( includedFile.find_last_of('.') + 1 );
			// Excluding map Files for now
			if( extension != "dmm") {
			    mFileNames.push_back( std::make_pair(includedFile, newDirectory) );
			}
		    }
		}
		failureLog << token->str() << std::endl;
		subtokens.emplace_back( std::move( token ) );
	    }
	}
	mFileTokens.emplace( file,
			     Token::create( Token::File, std::move( subtokens ) ) );

    }
    catch( ParseException& parseException ) {
	std::cout << parseException.toError() << std::endl;
	std::cout << failureLog.str() << std::endl << std::endl;
	return false;
    }
    return true;
}

void FileReader::run() {
    uint64_t success = 0;
    uint64_t total = 0;
    while(mFileNames.size() > 0 ) {
	std::string fileName = std::get<0>(mFileNames.back());
	std::string directory = std::get<1>(mFileNames.back());
	mFileNames.pop_back();
	if( mFileTokens.find( fileName ) == mFileTokens.end() ) {
	    if( readFile( fileName,
			  directory ) ) {
		success++;
	    }
	    total++;
	}
    }
    std::cout << success << "/" << total << "(" << 100 * success/total << "%) suceeded!" << std::endl;
}
