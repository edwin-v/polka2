#include "Functions.h"
#include <sstream>
#include <glibmm/miscutils.h>


std::vector<std::string> split(const std::string& s, char delim )
{
	std::vector<std::string> result;
	int p = 0;
	while( true ) {
		std::string::size_type l = s.find(delim, p);
		// break if out of loop
		if( l == std::string::npos ) break;
		// add
		result.push_back( s.substr( p, l-p ) );
		p = l+1;
	}
	result.push_back( s.substr(p) );
	return result;
}


#define SPACES " \t\r\n"

std::string trim_right( const std::string & s )
{
    std::string d(s);
    std::string::size_type i(d.find_last_not_of(SPACES));
    if( i == std::string::npos )
        return "";
    else
        return d.erase( d.find_last_not_of(SPACES) + 1 ) ;
}  

std::string trim_left( const std::string & s )
{
    std::string d(s);
    return d.erase( 0, s.find_first_not_of(SPACES) ) ;
}  

std::string trim( const std::string & s )
{
    std::string d(s);
    return trim_left( trim_right(d) ) ;
}  

double to_double( const std::string& s )
{
	std::istringstream ss(s);
	double result;
	if( ss >> result ) return result;
	return 0;
}

int to_int( const std::string& s )
{
	std::istringstream ss(s);
	int result;
	if( ss >> result ) return result;
	return 0;
}

bool checkObjectType( const std::string& type, const std::string& typespec )
{
	std::vector<std::string> reqs = split( typespec, ',' );
	for( std::vector<std::string>::iterator it = reqs.begin(); it != reqs.end(); ++it ) {
		*it = trim(*it);
		int sz = it->size();
		bool cat = it->at(sz-1) == '/';
		if( cat ) {
			if( type.compare(0, sz, *it) == 0 ) return true;
		} else {
			if( type == *it ) return true;
		}
	}
	return false;
}

Glib::ustring getNameFromFilename( const Glib::ustring& filename )
{
	Glib::ustring r = Glib::path_get_basename(filename);
	Glib::ustring::size_type dot = r.rfind('.');
	if( dot > 0 )
		return r.substr(0, dot);
	else
		return r;
}
