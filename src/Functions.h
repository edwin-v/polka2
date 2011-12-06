#ifndef _POLKA_FUNCTIONS_H_
#define _POLKA_FUNCTIONS_H_

#include <glibmm/ustring.h>
#include <string>
#include <vector>


std::vector<std::string> split(const std::string& s, char delim );
std::string trim_right( const std::string& s );
std::string trim_left( const std::string& s );
std::string trim( const std::string& s );
double to_double( const std::string& s );
int to_int( const std::string& s );

bool checkObjectType( const std::string& type, const std::string& typespec ); 

Glib::ustring getNameFromFilename( const Glib::ustring& filename );

template<typename T> 
T sqr( T v ) {
	return v*v;	
}

#endif
