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

#ifndef _POLKA_STORAGE_H_
#define _POLKA_STORAGE_H_

#include <string>
#include <vector>

namespace Polka {

class Storage
{
public:
	Storage( const std::string filename = "" );
	~Storage();

	// versioning
	void setFileIdentification( const std::string& id, int major, int minor );
	bool getFileIdentification( const std::string& id );
	int versionMajor() const;
	int versionMinor() const;
	int version() const;

	// main entries
	int load();
	int save();
	void setFilename( const std::string& filename );

	// item/object
	bool findItem( const std::string& name );
	bool findObject( const std::string& type = "" );
	bool findNextObject( const std::string& type = "" );
	void createItem( const std::string& name, const std::string& format, int size = -1 );
	Storage& createObject( const std::string& type );
	Storage& object();

	// validate
	bool checkFormat( const std::string& format ) const;
	int arraySize() const;
	bool isObject() const;
	const std::string& objectType() const;

	// access fields
	const std::string& stringField( int id );
	const std::string& stringField( int row, int id );
	int integerField( int id );
	int integerField( int row, int id );
	double floatField( int id );
	double floatField( int row, int id );
	const std::string& dataField( int id );
	const std::string& dataField( int row, int id );

	// store fields
	void setField( int id, const std::string& str );
	void setField( int row, int id, const std::string& str );
	void setField( int id, int val );
	void setField( int row, int id, int val );
	void setField( int id, double val );
	void setField( int row, int id, double val );
	std::string& setDataField( int id );
	std::string& setDataField( int row, int id );
	
	// deletion
	bool deleteObject( const std::string& type = "" );
	
	// error codes
	enum ErrorCodes { EFAILEDOPENWRITE = 1, EFAILEDOPENREAD, EFAILSTOREOBJECT,
	                  EITEMWITHOUTSEPARATOR, EBADITEMLINE, EBADITEMFORMAT,
	                  EDATAFIELDEMPTY, EPREMATUREENDDATA, EINVALIDDATA,
	                  EMISSINGDATAFATAL, EMISSINGDATANONFATAL, EINCORRECTDATATYPE,
	                  EINCORRECTDATALENGTH, EPREMATUREENDARRAY };
	
	
private:
	
	class Item
	{
	public:
		Item( const std::string _name, const std::string _format );
		~Item();

		
		const std::string& name() const;
		const std::string& format() const;
		
		// item definition
		void forceArraySize( int rows );
		void resetFormat( const std::string _format );
		
		// access fields
		const std::string& stringField( int id );
		const std::string& stringField( int row, int id );
		int integerField( int id );
		int integerField( int row, int id );
		double floatField( int id );
		double floatField( int row, int id );
		const std::string& dataField( int id );
		const std::string& dataField( int row, int id );

		bool isArray() const;
		int arraySize() const;
		bool isObject() const;
		Storage& object();

		// store fields
		void setField( int id, const std::string& str );
		void setField( int row, int id, const std::string& str );
		void setField( int id, int val );
		void setField( int row, int id, int val );
		void setField( int id, double val );
		void setField( int row, int id, double val );
		std::string& setDataField( int id );
		std::string& setDataField( int row, int id );
		
		// storage
		int load( std::istream& f );
		int save( std::ostream& f );
		
	private:
		// don't allow copy construction
		Item( const Item& _item );

		std::string m_Name;
		std::string m_Format, m_ArrayFormat;

		int m_ArraySize, m_RowSize;
		char *m_pData;
		std::vector<int> m_FieldLocs;
		std::vector<std::string> m_Data;

		Storage *m_pObject;

		bool mustEncode( const std::string& str );
		void base64encode( const std::string& str, std::ostream& f );
		bool base64decode( const std::string& str, unsigned int p, std::string& result );


	};

	std::string m_FileName;
	std::string m_Name;
	std::vector<Item*> m_Items;
	std::vector<Item*>::iterator m_itCurItem;
	Item *m_CurItem;
	int m_VersionMajor, m_VersionMinor;
	const Storage *m_pParent;

	int load( std::ifstream& f );
	int save( std::ofstream& f );
	
	void setParent( const Storage* s );
};

} // namespace Polka

#endif // _POLKA_STORAGE_H_


/*

Storage format:

Items:

*NAME:FIELDTYPES (I = integer, F = float, S = string/data (base64 encoded)
data
data
....


Objects:

+NAME
(items)
-NAME


Example:
*POLKA_FILE_VERSION:II
1, 0
*MAIN_WINDOW_GEOMETRY:IIII
200, 213
842, 712
+PAL2
*NAME:S
Level palettes/Level 1
*RGB:[FFF]
0.0, 0.0, 0.0
0.1, 0.5, 0.7
-PAL2


*/
