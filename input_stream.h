#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include <fstream>

/**
 * A simple charbased Stream
 **/
class InputStream
{
public:
    //! Constructor
    InputStream( std::string fileName );
    //! Destructor
    ~InputStream();
    //! Move-Constructor (default)
    InputStream( InputStream && rhs );

    //! Gives back the next character and increments the positon in the stream
    char next();
    //! Gives back the character offset from here without incrementing
    char peek( size_t offset = 0 );
    //! Returns whether or not we are at the end of the stream
    bool eof() const;
    //! Gives the fileName of the string
    std::string getFileName() const;
    //! Gives the current Column
    uint32_t getColumn() const;
    //! Gives the current Line
    uint32_t getLine() const;
private:
    //! Copy-Constructor (delete)
    InputStream( const InputStream & ) = delete;
    //! Asignment-Operator (delete)
    InputStream &operator=( const InputStream & ) = delete;

    char * mBuffer;
    size_t mPosition;
    size_t mLength;
    std::string mFileName;
    uint32_t mLine;
    uint32_t mColumn;
};

#endif // INPUT_STREAM_H
