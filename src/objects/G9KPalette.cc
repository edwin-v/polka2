#include "G9KPalette.h"
#include "Types.h"


namespace Polka {


const Byte G9K_PALETTE[16][3] = {
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


G9KPalette::G9KPalette( Project& _prj )
	: Palette( _prj, "PAL9", 5 )
{
	for( int c = 0; c < 16; c++ )
		initColor( c, G9K_PALETTE[c][0]*4+3,
		              G9K_PALETTE[c][1]*4+3,
		              G9K_PALETTE[c][2]*4+3 );
}

G9KPalette::~G9KPalette()
{
}

} // namespace Polka
