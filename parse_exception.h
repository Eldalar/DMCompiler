#ifndef PARSE_EXCEPTION_H
#define PARSE_EXCEPTION_H

#include <string>

/**
 * Zeigt an, dass beim parsen etwas schief ging
 **/
class ParseException
{
public:
    //! Constructor
    ParseException( std::string description,
		    std::string file,
		    uint32_t line,
		    uint32_t column );
    //! Destructor
    ~ParseException();
    //! Move-Constructor (default)
    ParseException( ParseException && ) = default;

    //! Gibt den vollen ErrorText zurück
    std::string toError() const;
private:
    //! Copy-Constructor (delete)
    ParseException( const ParseException & ) = delete;
    //! Asignment-Operator (delete)
    ParseException &operator=( const ParseException & ) = delete;

    std::string mDescription;
    std::string mFile;
    uint32_t mLine;
    uint32_t mColumn;
};

#endif // PARSE_EXCEPTION_H
