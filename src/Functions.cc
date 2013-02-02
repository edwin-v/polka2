/*
	Copyright (C) 2013 Edwin Velds

    This file is part of Polka 2.

    Polka 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Polka 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Polka 2.  If not, see <http://www.gnu.org/licenses/>.
*/

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
