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
	: Palette( _prj, "PAL1", 8 )
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
