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
