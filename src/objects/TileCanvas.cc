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

#include "TileCanvas.h"
#include "Palette.h"
#include "Project.h"
#include <cstring>
#include <assert.h>


namespace Polka {

TileCanvas::TileCanvas( Project& _prj )
	: Object( _prj, "TILECANVAS" )
{
	// create 24 bit bitmap with 32 bit int data XXRRGGBB
	// XX is used to store msx pattern and colour data in the first
	// pixels of the set of 8.
	m_Image = Cairo::ImageSurface::create( Cairo::FORMAT_RGB24, 32*8, 32*8 );
	assert( (m_Image->get_stride() & 3)==0 );

	// init image with palette colour zero
	int col =   ( int(round(m_Palette->r(0)*255)) << 16 )
	          + ( int(round(m_Palette->g(0)*255)) << 8 )
	          + ( int(round(m_Palette->b(0)*255)) );
	unsigned char *data = m_Image->get_data();
	unsigned int *idata = (unsigned int*)data;
	idata[0] = col;
	memcpy( data+4, data, 32*8*m_Image->get_stride()-4 );
}

TileCanvas::~TileCanvas()
{
	/* unset palette usage */
}

int TileCanvas::width() const
{
	return m_Image->get_width()/8;
}

int TileCanvas::height() const
{
	return m_Image->get_height()/8;
}

Cairo::RefPtr<Cairo::ImageSurface> TileCanvas::getImage() const
{
	return m_Image;
}

void TileCanvas::setPalette( const Palette& palette )
{
	setDependency( DEP_TCPAL, &palette );
	// update picture
	doUpdate();
}
	
void TileCanvas::doUpdate()
{
	// rewrite all image data
	int addr = 0;
	unsigned int *data = (unsigned int*)m_Image->get_data();
	// loop over all pixels in steps of 8
	for( int y = 0; y < m_Image->get_height(); y++ ) {
		for( int x = 0; x < m_Image->get_width(); x += 8 ) {
			unsigned char pat = (data[addr+x] >> 24);
			int fcol = (data[addr+x+1] >> 28);
			int bcol = (data[addr+x+1] >> 24) & 15;
			for( int p = 0, m = 128; p < 8; p++, m >>= 1 ) {
				// foreground or background
				int col = pat&m ? fcol : bcol;
				// write colour from palette
				data[addr+x+p] = (data[addr+x+p] & 0xFF000000)
					+ ( int(round(m_Palette->r(col)*255)) << 16 )
					+ ( int(round(m_Palette->g(col)*255)) << 8 )
					+ ( int(round(m_Palette->b(col)*255)) );
			}
		}
		addr += m_Image->get_stride()/4;
	}
}

const Palette *TileCanvas::getPalette() const
{
	return m_Palette;
}


} // namespace Polka
