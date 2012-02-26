#include "Storage.h"
#include "Functions.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace Polka {

static const std::string EMPTY;

/*
 * Storage class implementation
 */

Storage::Storage( const std::string filename )
	: m_FileName( filename ), m_CurItem(0), m_VersionMajor(-1), m_VersionMinor(-1), m_pParent(0)
{
 std::cout << "storage for " << m_FileName << std::endl;
}

Storage::~Storage()
{
	// delete items
	for( unsigned int i=0; i < m_Items.size(); i++ )
		delete m_Items[i];
}

void Storage::setFileIdentification( const std::string& id, int major, int minor )
{
	assert( minor < 1000 ); // limit minor version
	createItem( id, "II" );
	setField( 0, major );
	setField( 1, minor );
	m_VersionMajor = major;
	m_VersionMinor = minor;
}

bool Storage::getFileIdentification( const std::string& id )
{
	if( findItem(id) ) {
		if( m_CurItem->format() == "II" ) {
			m_VersionMajor = integerField(0);
			m_VersionMinor = integerField(1);
			return true;
		}
	}
	return false;
}

int Storage::versionMajor() const
{
	if( m_pParent )
		return m_pParent->versionMajor();
	else
		return m_VersionMajor;
}
	
int Storage::versionMinor() const
{
	if( m_pParent )
		return m_pParent->versionMinor();
	else
		return m_VersionMinor;
}

int Storage::version() const
{
	if( m_pParent )
		return m_pParent->version();
	else
		return 1000*m_VersionMajor+m_VersionMinor;
}


int Storage::load()
{
	std::cout << "obj: " << this << std::endl;
	std::ifstream file;
	file.open( m_FileName.c_str() ); std::cout << "loading " << m_FileName << std::endl;
	if( !file.is_open() ) return EFAILEDOPENREAD;
	return load(file);
}

int Storage::save()
{
	std::cout << "obj: " << this << std::endl;
	if( m_FileName.empty() ) m_FileName = "ALARM";
	std::ofstream file;
	file.open( m_FileName.c_str() ); std::cout << "saving " << m_FileName << std::endl;
	if( !file.is_open() ) return EFAILEDOPENWRITE;
	return save(file);
}

void Storage::setFilename( const std::string& filename )
{
	m_FileName = filename;
}

int Storage::load( std::ifstream& f )
{
	std::string buf;
	while( !f.eof() ) {
		// get a full line
		std::getline(f, buf);
 		buf = trim(buf);
		// check possibilites
		if( buf.empty() ) {
			// empty line, skip
			continue;
		} else if( buf[0] == '*' ) {
			// found name record
			size_t n = buf.find_first_of(':');
			if( n == std::string::npos ) {
				// no format separator
				return EITEMWITHOUTSEPARATOR;
			}
			std::string name = buf.substr( 1, n-1 );
			std::string format = buf.substr( n+1 );
			// check string sizes
			if( !name.size() || !format.size() ) return EBADITEMLINE;
			// create item first and check format result
			createItem( name, format );
			if( !m_CurItem->format().size() ) return EBADITEMFORMAT;
			// read item
			int err = m_CurItem->load(f);
			if( err ) return err;
			
		} else if( buf[0] == '+' ) {
			// found object
			Storage& subS = createObject( trim(buf.substr(1)) );
			int err = subS.load( f );
if( err ) std::cout << "Error " << err << " in loading of object " << buf << std:: endl;
		} else if( buf[0] == '-' ) {
			// found object end marker
			return 0;
		} else {
			// found garbage => error
			return EINVALIDDATA;
		}
	}
	
	return 0;
}

int Storage::save( std::ofstream& f )
{
	m_itCurItem = m_Items.begin();
	while( m_itCurItem != m_Items.end() ) {
		m_CurItem = *m_itCurItem;
		if( m_CurItem->isObject() ) {

			// save object name
			f << '+' << m_CurItem->name() << std::endl;
			// save object
			int err = m_CurItem->object().save(f);
			if( err ) return EFAILSTOREOBJECT;
			// end object
			f << '-' << m_CurItem->name() << std::endl;
	
		} else {

			// save item name
			f << "*" << m_CurItem->name() << ":" << m_CurItem->format() << std::endl;
			m_CurItem->save(f);
			
		}
		++m_itCurItem;
	}
	return 0;	
}

bool Storage::findItem( const std::string& name )
{
	m_itCurItem = m_Items.begin();
	while( m_itCurItem != m_Items.end() ) {
		if( (*m_itCurItem)->name() == name ) {
			m_CurItem = *m_itCurItem;
			return true;
		}
		++m_itCurItem;
	}
	m_CurItem = 0;
	return false;	
}

bool Storage::findObject( const std::string& type )
{
	m_itCurItem = m_Items.begin();
	while( m_itCurItem != m_Items.end() ) {
		if( (*m_itCurItem)->isObject() && (type.empty() || (*m_itCurItem)->name() == type) ) {
			m_CurItem = *m_itCurItem;
			return true;
		}
		++m_itCurItem;
	}
	m_CurItem = 0;
	return false;
}

bool Storage::findNextObject( const std::string& type )
{
	++m_itCurItem;
	while( m_itCurItem != m_Items.end() ) {
		if( (*m_itCurItem)->isObject() && (type.empty() || (*m_itCurItem)->name() == type) ) {
			m_CurItem = *m_itCurItem;
			return true;
		}
		++m_itCurItem;
	}
	m_CurItem = 0;
	return false;
}

void Storage::createItem( const std::string& name, const std::string& format, int size )
{ std::cout << "CREATE: " << name << ", " << format << std::endl;
	if( findItem( name ) ) {
		m_CurItem->resetFormat( format );
	} else {
		m_CurItem = new Item( name, format );
		m_Items.push_back( m_CurItem );
		m_itCurItem = m_Items.end();
		--m_itCurItem;
	}
	if( size > 0 )
		if( m_CurItem->isArray() ) 
			m_CurItem->forceArraySize(size);
}

Storage& Storage::createObject( const std::string& type )
{ std::cout << "create object: " << type << std::endl;
	// create new object item
	m_CurItem = new Item( type, "O" );
	m_Items.push_back( m_CurItem );
	m_CurItem->object().setParent(this);
	return m_CurItem->object();
}

Storage& Storage::object()
{
	assert( m_CurItem );
	return m_CurItem->object();
}

bool Storage::deleteObject( const std::string& type )
{
	if( !type.empty() ) findObject(type);
	if( !isObject() ) return false;
	// delete
	delete m_CurItem;
	m_CurItem = 0;
	m_Items.erase(m_itCurItem);
	m_itCurItem = m_Items.end();
	return true;
}

bool Storage::checkFormat( const std::string& format ) const
{
	if( !m_CurItem ) return false;
	
	return m_CurItem->format() == format;	
}

int Storage::arraySize() const
{
	if( !m_CurItem ) return false;
	
	return m_CurItem->arraySize();
}

bool Storage::isObject() const
{
	if( !m_CurItem ) return false;
	return m_CurItem->isObject();
}

const std::string& Storage::objectType() const
{
	assert( m_CurItem );
	assert( m_CurItem->isObject() );
	return m_CurItem->name();
}


// access functions
const std::string& Storage::stringField( int id )
{
	assert( m_CurItem );
	return m_CurItem->stringField(id);
}

const std::string& Storage::stringField( int row, int id )
{
	assert( m_CurItem );
	return m_CurItem->stringField(row, id);
}

int Storage::integerField( int id )
{
	assert( m_CurItem );
	return m_CurItem->integerField(id);
}

int Storage::integerField( int row, int id )
{
	assert( m_CurItem );
	return m_CurItem->integerField(row, id);
}

double Storage::floatField( int id )
{
	assert( m_CurItem );
	return m_CurItem->floatField(id);
}

double Storage::floatField( int row, int id )
{
	assert( m_CurItem );
	return m_CurItem->floatField(row, id);
}

const std::string& Storage::dataField( int id )
{
	assert( m_CurItem );
	return m_CurItem->dataField(id);
}

const std::string& Storage::dataField( int row, int id )
{
	assert( m_CurItem );
	return m_CurItem->dataField(row, id);
}

// storage functions
void Storage::setField( int id, const std::string& str )
{
	assert( m_CurItem );
	m_CurItem->setField(id, str);
}

void Storage::setField( int row, int id, const std::string& str )
{
	assert( m_CurItem );
	m_CurItem->setField(row, id, str);
}

void Storage::setField( int id, int val )
{
	assert( m_CurItem );
	m_CurItem->setField(id, val);
}

void Storage::setField( int row, int id, int val )
{
	assert( m_CurItem );
	m_CurItem->setField(row, id, val);
}

void Storage::setField( int id, double val )
{
	assert( m_CurItem );
	m_CurItem->setField(id, val);
}

void Storage::setField( int row, int id, double val )
{
	assert( m_CurItem );
	m_CurItem->setField(row, id, val);
}

std::string& Storage::setDataField( int id )
{
	assert( m_CurItem );
	return m_CurItem->setDataField(id);
}

std::string& Storage::setDataField( int row, int id )
{
	assert( m_CurItem );
	return m_CurItem->setDataField(row, id);
}

void Storage::setParent( const Storage* s )
{
	m_pParent = s;
}


/*
 * Item class
 */

Storage::Item::Item( const std::string _name, const std::string _format )
	: m_pData(0), m_pObject(0)
{
	m_Name = trim(_name);
	
	if( _format[0] == 'O' ) {
		
		m_pObject = new Storage;
		m_Format = "O";
		m_ArraySize = -1;
		
	} else {
		m_pObject = 0;	
		resetFormat( _format );
	}
}

Storage::Item::Item( const Item& _item )
{
	assert(false);
}

Storage::Item::~Item()
{ std::cout << "~Item: " << m_Name << std::endl;
	if( m_pData ) free(m_pData);
	if( m_pObject ) delete m_pObject;
}

const std::string& Storage::Item::name() const
{
	return m_Name;
}

const std::string& Storage::Item::format() const
{
	return m_ArraySize>=0 ? m_ArrayFormat : m_Format;
}

void Storage::Item::resetFormat( const std::string _format )
{
	// determine array size
	m_ArraySize = _format[0]=='['?0:-1;
	
	// store format
	if( m_ArraySize >= 0 ) {
		m_Format = _format.substr(1, _format.size()-2);
		m_ArrayFormat = _format;
	} else 
		m_Format = _format;

	// check fields
	assert( m_Format.find_first_not_of("IFS") == std::string::npos );
		
	// init item data
	if( m_pData ) free( m_pData );
	m_pData = 0;
	m_FieldLocs.clear();
	m_FieldLocs.push_back(0);
	m_Data.clear();
	for( unsigned int i = 0; i < m_Format.size(); i++ ) {
		if( i == m_Format.size()-1 ) {
			// last  field determines size
			m_RowSize = m_FieldLocs.back() + (m_Format[i]=='F'?sizeof(double):sizeof(int));
		} else {
			m_FieldLocs.push_back( m_FieldLocs.back() + (m_Format[i]=='F'?sizeof(double):sizeof(int)) );
		}
	}
	
	// allocate one row
	m_pData = (char*)malloc(m_RowSize);
	memset( m_pData, 255, m_RowSize );
}

bool Storage::Item::isArray() const
{
	return m_ArraySize >= 0;
}

int Storage::Item::arraySize() const
{
	assert( m_ArraySize >= 0 );
	return m_ArraySize;
}

bool Storage::Item::isObject() const
{
	return m_pObject != 0;
}

Storage& Storage::Item::object()
{
	assert( m_pObject );
	return *m_pObject;
}

void Storage::Item::forceArraySize( int rows )
{
	assert( m_ArraySize >= 0 );
	if( rows > m_ArraySize  ) {
		m_pData = (char*)realloc( m_pData, m_RowSize*rows );
		memset( m_pData + m_ArraySize*m_RowSize, 255, (rows-m_ArraySize)*m_RowSize );
		m_ArraySize = rows;
	}
}

// ****************
// access functions
// ****************
const std::string& Storage::Item::stringField( int id )
{
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'S' );
	// get string id
	int sid = *(int *)(m_pData + m_FieldLocs[id]);
	// return data
	if( sid == -1 )
		return EMPTY;
	else
		return m_Data[sid];
}

const std::string& Storage::Item::stringField( int row, int id )
{
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'S' );
	// check array size
	if( row >= m_ArraySize ) return EMPTY;
	// get string id
	int sid = *(int *)(m_pData + m_RowSize*row + m_FieldLocs[id]);
	// return data
	if( sid == -1 )
		return EMPTY;
	else
		return m_Data[sid];
}

const std::string& Storage::Item::dataField( int id )
{
	return stringField(id);
}

const std::string& Storage::Item::dataField( int row, int id )
{
	return stringField(row, id);
}

int Storage::Item::integerField( int id )
{
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'I' );
	// get int
	return *(int*)( m_pData + m_FieldLocs[id] );
}

int Storage::Item::integerField( int row, int id )
{
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'I' );
	// check array size
	if( row >= m_ArraySize ) return 0;
	// get int
	return *(int*)( m_pData + m_RowSize*row + m_FieldLocs[id] );
}

double Storage::Item::floatField( int id )
{
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'F' );
	// get double
	return *(double*)( m_pData + m_FieldLocs[id] );
}

double Storage::Item::floatField( int row, int id )
{
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'F' );
	// check array size
	if( row >= m_ArraySize ) return 0;
	// get int
	return *(double*)( m_pData + m_RowSize*row + m_FieldLocs[id] );
}

// *****************
// storage functions
// *****************
void Storage::Item::setField( int id, const std::string& str )
{
	assert( m_ArraySize == -1 );
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'S' );
	// store string id
	int *dat = (int *)(m_pData + m_FieldLocs[id]);
	// create data field if not available
	if( *dat == -1 ) { *dat = m_Data.size(); m_Data.push_back( std::string() ); }
	m_Data[*dat] = str;
}

void Storage::Item::setField( int row, int id, const std::string& str )
{
	assert( m_ArraySize >= 0 );
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'S' );
	// correct array size
	forceArraySize(row+1);
	// store string id
	int *dat = (int *)(m_pData + m_RowSize*row + m_FieldLocs[id]);
	// create data field if not available
	if( *dat == -1 ) { *dat = m_Data.size(); m_Data.push_back( std::string() ); }
	m_Data[*dat] = str;
}

void Storage::Item::setField( int id, int val )
{
	assert( m_ArraySize == -1 );
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'I' );
	// store int
	*(int *)(m_pData + m_FieldLocs[id]) = val;
}

void Storage::Item::setField( int row, int id, int val )
{
	assert( m_ArraySize >= 0 );
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'I' );
	// correct array size
	forceArraySize(row+1);
	// store int
	*(int *)(m_pData + m_RowSize*row + m_FieldLocs[id]) = val;
}

void Storage::Item::setField( int id, double val )
{
	assert( m_ArraySize == -1 );
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'F' );
	// store int
	*(double *)(m_pData + m_FieldLocs[id]) = val;
}

void Storage::Item::setField( int row, int id, double val )
{
	assert( m_ArraySize >= 0 );
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'F' );
	// correct array size
	forceArraySize(row+1);
	// store int
	*(double *)(m_pData + m_RowSize*row + m_FieldLocs[id]) = val;
}

std::string& Storage::Item::setDataField( int id )
{
	assert( m_ArraySize == -1 );
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'S' );
	// store string id
	int *dat = (int *)(m_pData + m_FieldLocs[id]);
	// create data field if not available
	if( *dat == -1 ) { *dat = m_Data.size(); m_Data.push_back( std::string() ); }
	return m_Data[*dat];
}

std::string& Storage::Item::setDataField( int row, int id )
{
	assert( m_ArraySize >= 0 );
	assert( id < int(m_Format.size()) );
	assert( m_Format[id] == 'S' );
	// correct array size
	forceArraySize(row+1);
	// store string id
	int *dat = (int *)(m_pData + m_RowSize*row + m_FieldLocs[id]);
	// create data field if not available
	if( *dat == -1 ) { *dat = m_Data.size(); m_Data.push_back( std::string() ); }
	return m_Data[*dat];
}


/*
 * base64
 * 
 *   Encode string to base64 (6 bit to "A-Za-z0-9+/")
 */
void Storage::Item::base64encode( const std::string& str, std::ostream& f )
{
	int count = 0;
	for( int i = 0; i+2 < int(str.size()); i += 3 ) {
		// create 3 char value
		int n = ((unsigned char)(str[i])  << 16) |
		        ((unsigned char)(str[i+1]) << 8) | 
		         (unsigned char)(str[i+2]);

		// convert to 4 base64 chars
		for( int c = 18; c >= 0; c-=6 ) {
			char b = (n >> c) & 63;
			if( b < 26 )
				b += 'A';
			else if( b < 52 )
				b += 'a'-26;
			else if( b < 62 )
				b += '0'-52;
			else if( b == 62 )
				b = '+';
			else
				b = '/';
			f << b;
			count++;
		}
		// lines of 64
		if( count == 100 ) { f << std::endl; count = 0; }
	}
	// last bytes
	unsigned int N = str.size();
	if( N % 3 == 0 ) {
		// add an extra = if everything is done
		f << '=';
	} else {
		// 
		int n;
		if( N % 3 == 1 )
			n = (str[N-1] << 16) | 1;
		else 
			n = (str[N-2] << 16) | (str[N-1] << 8);

		for( int c = 3; c > (n&3); c-- ) {
			char b = (n >> (6*c)) & 63;
			if( b < 26 )
				b += 'A';
			else if( b < 52 )
				b += 'a'-26;
			else if( b < 62 )
				b += '0'-52;
			else if( b == 63 )
				b = '+';
			else
				b = '/';
			f << b;
		}
		// add end marker
		f << '=';
		if( N%3 == 1 ) f << '=';
	}
}

bool Storage::Item::base64decode( const std::string& str, unsigned int p, std::string& result )
{
	while( p < str.size() ) {
		// skip whitespace
		if( str[p] < '0' )
			p++;
		else if( str[p] == '=' )
			// no more quads
			p = str.size();
		else if( str.size() - p < 4 )
			// not enough chars till end
			p = str.size();
		else {
			// calc binary value
			int val = 0;
			for( int i = 0; i < 4; i++ ) {
				val <<= 6;
				int b = str[p + i];
				if( b >= 'A' && b <= 'Z' )
					val += b - 'A';
				else if( b >= 'a' && b <= 'z' )
					val += b - 'a' + 26;
				else if( b >= '0' && b <= '9' )
					val += b - '0' + 52;
				else if( b == '+' )
					val += 62;
				else
					val += 63;
			}
			// output char to string
			result += char(val >> 16);
			if( str[ p+2 ] == '=' )
				p = str.size();
			else {
				result += char( (val >> 8) & 255 );
				if( str[ p+3 ] == '=' )
					p = str.size();
				else 
					result += char( val & 255 );
			}
			p += 4;
		}
	}
	return true;
}

bool Storage::Item::mustEncode( const std::string& str )
{
	if( str.size() > 1024 ) return true;
	// find a specific foul characters
	return str.find_first_of("\000\007\008\027") != std::string::npos;
}

int Storage::Item::load( std::istream& f )
{
	// read items according to format
	int fld = 0, numFields = m_Format.size();
	// array can be empty
	if( m_ArraySize >= 0 ) {
		// peek next line for non data character
		char c = f.peek();
		if( c == '+' || c == '*' || c == '-')
			numFields = 0;
	}

	std::string line;
	size_t ptr = 0;
	while( fld < numFields ) {
		// skip white space
		if( ptr >= line.size() || (ptr = line.find_first_not_of(" \t\r\n", ptr)) == std::string::npos ) {
			// load next line from file
			if( f.eof() ) return EPREMATUREENDARRAY;
			std::getline(f, line);
			// check if field is not name/object
			if( line[0] == '*' || line[0] == '+' )
				return EPREMATUREENDARRAY;
			// start 
			ptr = line.find_first_not_of(" \t\r\n");
			// error if no data found
			if( ptr == std::string::npos )
				return EPREMATUREENDARRAY;
		}
		// store field
		int id = fld%m_Format.size(), row = fld/m_Format.size();
		switch( m_Format[id] ) {
			case 'I':
			{
				int pos = line.find(',', ptr);
				std::string field = line.substr(ptr, pos-ptr);
				int r;
				std::istringstream ss( field );
				ss >> r;
				if( m_ArraySize == -1 )
					setField( id, r );
				else
					setField( row, id, r );
				ptr = pos;
				break;
			}
			case 'F':
			{
				int pos = line.find(',', ptr);
				std::string field = line.substr(ptr, pos-ptr);
				double r;
				std::istringstream ss( field );
				ss >> r;
				if( m_ArraySize == -1 )
					setField( id, r );
				else
					setField( row, id, r );
				ptr = pos;
				break;
			}
			default:
				// string/data
				std::string& str = m_ArraySize==-1?setDataField(id):setDataField(row, id);
				if( line[ptr] == '"' ) {
					// create string
					ptr++;
					while(true) {
						// find quotes or end of line
						size_t end = line.find('"', ptr);
					
						if( end == std::string::npos ) {
							// end of line, copy everything
							str += line.substr(ptr);
							str += "\n";
							// and continue on next line
							if( f.eof() ) return EPREMATUREENDDATA;
							std::getline(f, line);
							ptr = 0;
						} else if( int(end) == int(line.size())-1 || line[end+1] != '"') {
							// end of string found
							str += line.substr(ptr, end-ptr);
							ptr = end+1;
							break;
						} else {
							// double quote
							str += line.substr(ptr, 1+end-ptr);
							ptr = end+2;
						}
						
					}
				} else {
					// create data
					size_t end;
					while(true) {
						if( !base64decode( line, ptr, str ) )
							return EINVALIDDATA;
						end = line.find('=', ptr);
						if( end != std::string::npos ) {
							ptr = end+1;
							while( line[ptr] == '=' ) ptr++;
							break;
						}
						// get next line
						if( f.eof() ) return EPREMATUREENDDATA;
						std::getline(f, line);
					}
				}
		}

		// field done
		fld++;
		// ptr after field, more data?
		ptr = line.find_first_not_of(" \t\n\r", ptr);
		if( ptr == std::string::npos ) 
			break;

		// more data
		ptr++;
		
		// extend field count for arrays
		if( fld == numFields )
			if( m_ArraySize >= 0 )
				numFields += m_Format.size();
	}
	
	if( fld != numFields )
		return EPREMATUREENDDATA;
	
	return 0;
}

int Storage::Item::save( std::ostream& f )
{
	if( m_ArraySize == 0 ) return 0;

	int p = 0;
	int num = m_Format.size();
	if( m_ArraySize >= 0 ) num *= m_ArraySize;

	for( int i = 0; i < num; i++ ) {
		// calc ids
		int id = i%m_Format.size(), row = i/m_Format.size();

		// add newline for array readability
		if( m_ArraySize >= 0 && p && id == 0 ) {
			f << std::endl;
			p = 0;
		}

		// add safety space
		if( p == 0 && m_Format[id] != 'S' ) f << ' ';
		switch( m_Format[id] ) {
			case 'I':
			{
				int st = int(f.tellp());
				f << *(int*)(m_pData + row*m_RowSize + m_FieldLocs[id]);
				p += int(f.tellp()) - st;
				break;
			}
			case 'F':
			{
				int st = int(f.tellp());
				f << *(double*)(m_pData + row*m_RowSize + m_FieldLocs[id]);
				p += int(f.tellp()) - st;
				break;
			}
			default:
			{
				// get data id
				int datId = *(int*)(m_pData + row*m_RowSize + m_FieldLocs[id]);
				if( datId == -1 ) {
					f << "\"\"";
					p += 2;
				} else if( mustEncode( m_Data[datId] ) ) {
					// base 64 encode output
					base64encode( m_Data[datId], f );
					p = 100;
				} else {
					p += 2 + m_Data[datId].size();
					f << "\"";
					int l = 0;
					while(true) {
						size_t e = m_Data[datId].find( '"', l );
						if( e == std::string::npos ) {
							f << m_Data[datId].substr(l);
							break;
						} else {
							f << m_Data[datId].substr(l, e-l+1) << '"';
							l = e+1;
							p++;
						}
					}
					f << "\"";
				}
				break;
			}
		}
		// add comma for more data
		if( i < num-1 ) {
			f << ", ";
			p += 2;
			if( p >= 100 ) {
				f << std::endl;
				p = 0;
			}
		}
	}
	// end with new line
	f << std::endl;
	return 0;
}


} // namespace ...

