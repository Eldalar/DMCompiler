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
	std::vector<TokenPtr> tokens;
	bool preProcessorFound = false;
	while( !tokenStream.eof() ) {
	    TokenPtr token = tokenStream.next();
	    if( token->getType() == Token::Preprocessor &&
		token->getContent() == "include" ) {
		preProcessorFound = true;
	    } else if( preProcessorFound ) {
		if( token->getType() == Token::String ) {
		    mFileNames.push_back( make_pair( token->getContent(),
							newDirectory) );
		    preProcessorFound = false;
		} else if( token->getType() != Token::Whitespace ) {
		    tokenStream.throwError( "include followed by something else than a string or a whitespace: " +
					    token->str());
		    preProcessorFound = false;
		}
	    }
	    failureLog << token->str() << std::endl;
	    tokens.emplace_back( std::move( token ) );
	}
	mFileTokens.emplace( file,
			     std::move( tokens ) );

    }
    catch( ParseException& parseException ) {
	std::cout << failureLog.str() << std::endl << std::endl;
	std::cout << parseException.toError() << std::endl;
	std::cout << "------------" << std::endl;
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
