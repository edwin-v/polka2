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

#ifndef _POLKA_PALETTE_H_
#define _POLKA_PALETTE_H_

#include "Object.h"
#include <vector>


namespace Polka {

class Palette : public Object 
{
public:
	Palette( Project& _prj, const std::string& _id, int depth, int size = 16 );
	virtual ~Palette();

	// palette modification
	void setColor( int nr, double r, double g, double b );
	void setColors( int nr, int count, double r[], double g[], double b[] );
	
	// palette access
	int depth() const;
	int size() const;
	double r( unsigned int nr ) const;
	double g( unsigned int nr ) const;
	double b( unsigned int nr ) const;
	
	// palette modification
	void copyColor( int src, int dest );
	void swapColor( int c1, int c2 );
	void createGradient( int c1, int c2 );

	// undo stuff
	virtual void undo( const std::string& id, Storage& s );
	virtual void redo( const std::string& id, Storage& s );

protected:
	void setSkipSave( bool value = true );
	void initColor( int nr, int r, int g, int b );

	// storage
	virtual int store( Storage& s );
	virtual int restore( Storage& s );

private:
	std::vector<double> m_Red, m_Green, m_Blue;
	std::vector<double> m_DispRed, m_DispGreen, m_DispBlue;
	int m_Depth;
	bool m_SkipSave;
	
	void doSwapColor( int n1, int n2 );
	void doCreateGradient( int c1, int c2 );
	void changeDisplayColors( int n1, int n2 = -1 );
	void storeColors( Storage& s, int n1 = -1, int n2 = -1 );
	int restoreColors( Storage& s );
	void performAction( const std::string& id, Storage& s );
};

} // namespace Polka

#endif // _POLKA_PALETTE_H_
