#include "parse_exception.h"
#include <sstream>

ParseException::ParseException( std::string description,
				std::string file,
				uint32_t line,
				uint32_t column )
    : mDescription( description ),
      mFile( file ),
      mLine( line ),
      mColumn( column ) {
}

ParseException::~ParseException() {
}

std::string ParseException::toError() const {
    std::stringstream ss;
    ss << mFile << "(" << mLine << ":" << mColumn << "): " << mDescription;
    return ss.str();
}
