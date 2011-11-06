#include "G9KPalette64.h"
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


G9KPalette64::G9KPalette64( Project& _prj )
	: Palette( _prj, "PAL964", 5, 64 )
{
	for( int c = 0; c < 64; c++ )
		initColor( c, G9K_PALETTE[c&15][0]*(4-c/16)+(3-c/16),
		              G9K_PALETTE[c&15][1]*(4-c/16)+(3-c/16),
		              G9K_PALETTE[c&15][2]*(4-c/16)+(3-c/16) );
}

G9KPalette64::~G9KPalette64()
{
}

} // namespace Polka
