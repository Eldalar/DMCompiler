#ifndef FILE_READER_H
#define FILE_READER_H

#include "token.h"

#include <string>
#include <vector>
#include <map>

/**
 * Reads the various files
 **/
class FileReader
{
public:
    //! Constructor
    FileReader( std::string folder, std::string mainFile );
    //! Destructor
    ~FileReader();
    //! Move-Constructor (default)
    FileReader( FileReader && ) = default;

    void run();
private:
    bool readFile( std::string file,
		   std::string directory);
    //! Copy-Constructor (delete)
    FileReader( const FileReader & ) = delete;
    //! Asignment-Operator (delete)
    FileReader &operator=( const FileReader & ) = delete;

    std::map<std::string, std::vector<TokenPtr>> mFileTokens;
    std::vector<std::pair<std::string,std::string>> mFileNames;
    std::string mFolder;
};

#endif // FILE_READER_H
