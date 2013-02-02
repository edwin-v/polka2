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

#include "MSX1Palette.h"
#include "Types.h"


namespace Polka {


const Byte TMS99X8A_PALETTE[16][3] = {
	{   0,   0,   0 },
	{   0,   0,   0 },
	{  33, 200,  66 },
	{  94, 220, 120 },
	{  84,  85, 237 },
	{ 125, 118, 252 },
	{ 212,  82,  77 },
	{  66, 235, 245 },
	{ 252,  85,  84 },
	{ 255, 121, 120 },
	{ 212, 193,  84 },
	{ 230, 206, 128 },
	{  33, 176,  59 },
	{ 201,  91, 186 },
	{ 204, 204, 204 },
	{ 255, 255, 255 }
};



MSX1Palette::MSX1Palette( Project& _prj )
	: Palette( _prj, MSX1PAL_ID, 8 )
{
	setSkipSave();
	for( int c = 0; c < 16; c++ )
		initColor( c, TMS99X8A_PALETTE[c][0],
		              TMS99X8A_PALETTE[c][1],
		              TMS99X8A_PALETTE[c][2] );
}

MSX1Palette::~MSX1Palette()
{
}

} // namespace Polka
