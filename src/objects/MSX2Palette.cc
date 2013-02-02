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

#include "MSX2Palette.h"
#include "Types.h"


namespace Polka {


const Byte V9938_PALETTE[16][3] = {
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 1, 6, 1 },
	{ 3, 7, 3 },
	{ 1, 1, 7 },
	{ 2, 3, 7 },
	{ 6, 1, 1 },
	{ 2, 6, 7 },
	{ 7, 1, 1 },
	{ 7, 3, 3 },
	{ 6, 6, 1 },
	{ 6, 6, 4 },
	{ 1, 4, 1 },
	{ 6, 2, 5 },
	{ 5, 5, 5 },
	{ 7, 7, 7 }
};


MSX2Palette::MSX2Palette( Project& _prj )
	: Palette( _prj, MSX2PAL_ID, 3 )
{
	for( int c = 0; c < 16; c++ )
		initColor( c, V9938_PALETTE[c][0],
		              V9938_PALETTE[c][1],
		              V9938_PALETTE[c][2] );
}

MSX2Palette::~MSX2Palette()
{
}

} // namespace Polka
