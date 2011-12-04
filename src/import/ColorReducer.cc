#include "ColorReducer.h"
#include "Functions.h"
#include "ObjectManager.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <list>

using namespace std;

namespace Polka {

unsigned char cclip( int v )
{
	if( v < 0 )
		return 0;
	else if( v > 255 )
		return 255;
	return v;
}
		
// error diffusion dither matrices

struct ErrorDiffusionMatrix {
	int width, height;
	int offset, div;
	unsigned char matrix[32];
};

const int NUM_ERROR_DIFFUSION = 1 + ColorReducer::DITHER_ATKINSON - ColorReducer::DITHER_FLOYDSTEINBERG;
ErrorDiffusionMatrix ErrorDiffusionMatrices[NUM_ERROR_DIFFUSION] =
{
	//Floyd-Steinberg:
	{ 3, 2, -1, 16, 
	  { 7,   3, 5, 1 }
	},
	 
	//Jarvis-Judice-Ninke:
	{ 5, 3, -2, 48,
	  { 7, 5,   3, 5, 7, 5, 3,   1, 3, 5, 3, 1 }
	},

	//Stucki:
	{ 5, 3, -2, 42 ,
	  {8, 4,   2, 4, 8, 4, 2,   1, 2, 4, 2, 1 }
	},
	
	//Burkes:
	{ 5, 2, -2, 32,
	  { 8, 4,   2, 4, 8, 4, 2 }
	},

	//Sierra3:
	{ 5, 3, -2, 32,
	  { 5, 3,   2, 4, 5, 4, 2,   0, 2, 3, 2, 0 }
	},

	//Sierra2:
	{ 5, 2, -2, 16,
	  { 4, 3,   1, 2, 3, 2, 1 }
	},

	//Sierra-2-4A:
	{ 3, 2, -1, 4,
	  { 2,   1, 1, 0 }
	},

	//Stevenson-Arce:
	{ 7, 4, -3, 200,
	  { 0, 32, 0,   12, 0, 26, 0, 30, 0, 16,   
	    0, 12, 0, 26, 0, 12, 0,   5, 0, 12, 0, 12, 0, 5 }
	},

	//Atkinson:
	{ 4, 3, -1, 8,
	  { 1, 1,    1, 1, 1, 0,   0, 1, 0, 0 }
	}
};


/*
 **********************************************************************
 * ColorReducer class
 * 
 * This class creates reduced color data objects.
 **********************************************************************
 */

// possible import targets
struct Target
{
	int Colors;
	int Depth;
	std::string Canvas;
};
static std::map<std::string, Target> Targets;


ColorReducer::ColorReducer()
	: m_pSource(0), m_pDest(0),
	  m_DitherType(DITHER_NONE),
	  m_QuantMethod(QUANT_OCTREE),
	  m_ColorErrorMethod(COLORERROR_RGB),
	  m_OrderX(2), m_OrderY(2),
	  m_Root(this)
{
	// TEMP: Add targets
	//Targets["PAL1"] = { 16, 3, "BMP16CANVAS"} );
	Targets["PAL2"] = { 16, 3, "BMP16CANVAS"};
	Targets["PAL9"] = { 16, 5, "BMP16CANVAS"};
}

ColorReducer::~ColorReducer()
{
}

void ColorReducer::getTargetPalettes( std::vector<std::string>& pal_vec )
{
	auto it = Targets.begin();
	while( it != Targets.end() ) {
		pal_vec.push_back( it->first );
		++it;
	}
}

const std::string& ColorReducer::getTargetCanvas( const std::string& pal )
{
	auto it = Targets.find(pal);
	if( it == Targets.end() ) return Targets[0].Canvas;
	
	return it->second.Canvas;
}

bool ColorReducer::needQuantization( const std::string& pal )
{
	auto it = Targets.find(pal);
	if( it == Targets.end() ) return false;
	
	return it->second.Colors < 256;
}

void ColorReducer::setRGBSource( const unsigned char *src, int width, int height, int pixsize, int stride )
{
	m_pSource = src;
	m_Width = width;
	m_Height = height;
	m_SrcPixSize = pixsize;
	if( stride > 0 )
		m_SrcStride = stride;
	else
		m_SrcStride = pixsize*width;
}

void ColorReducer::setTarget( const std::string& palette, unsigned char *dest, int pixsize, int stride )
{
	auto it = Targets.find(palette);

	assert( it != Targets.end() );

	m_pDest = dest;
	m_DstPixSize = pixsize;
	m_DstStride = stride;
	
	m_NumCols = it->second.Colors;
	m_Depth = it->second.Depth;
}

void ColorReducer::setDitherType( DitherType type, int orderx, int ordery, int offsetx, int offsety )
{
	m_DitherType = type;
	m_OrderX = orderx>0 ? orderx : 2;
	m_OrderY = ordery>0 ? ordery : 2;
	m_OffsetX = ( offsetx >=0 && offsetx < (1<<m_OrderX) ) ? offsetx : 0;
	m_OffsetY = ( offsety >=0 && offsety < (1<<m_OrderY) ) ? offsety : 0;
}

void ColorReducer::setQuantizationMethod( QuantizationMethod method, ColorErrorMethod error )
{
	m_QuantMethod = method;
	m_ColorErrorMethod = error;
}

int ColorReducer::colorError( int R1, int G1, int B1, int R2, int G2, int B2 )
{
	switch( m_ColorErrorMethod ) {
		case COLORERROR_RGB:
			// return simple squared difference
			return sqr(R2-R1)+sqr(G2-G1)+sqr(B2-B1);
		case COLORERROR_PERCEPTUAL:
			// calculate squared color error according to dE1994
			// Information and reference implementation:
			//    http://en.wikipedia.org/wiki/Color_difference
			//    http://en.wikipedia.org/wiki/Lab_color_space
			//    http://www.emanueleferonato.com/2009/09/08/color-difference-algorithm-part-2/

			// convert to Lab-color
			double L1, a1, b1, L2, a2, b2;
			rgbToLab( R1, G1, B1, L1, a1, b1 );
			rgbToLab( R2, G2, B2, L2, a2, b2 );
			double C1 = sqrt(a1*a1+b1*b1), C2 = sqrt(a2*a2+b2*b2);
			double deltaC = C1-C2;
			double deltaL = L1-L2, deltaa = a1-a2, deltab = b1-b2;
			double deltaH2 = deltaa*deltaa+deltab*deltab-deltaC*deltaC;
			double deltaH = deltaH2>0.0 ? sqrt(deltaH2) : 0.0;
			double deltaCK = deltaC/(1+0.045*C1), deltaHK = deltaH/(1+0.015*C1);
			double deltaE2 = deltaL*deltaL + deltaCK*deltaCK + deltaHK*deltaHK;
			// squared error samples:
			// Black/White:        10000
			// Black/Red:          13770
			// Red/White:          2524.0
			// Black/Green:        22051
			// Green/White:        502.11
			// Black/Blue:         18950
			// Blue/White:         4946.5
			// Black/010000:       0.080275
			// White/FEFFFF:       0.12300
			// Black/000100:       0.28100
			// White/FFFEFF:       0.42541
			// Black/000001:       0.16302
			// White/FFFFFE:       0.24736
			// Total error should represent all values comfortably.
			return round(deltaE2*75);
	}
	// unreachable
	return 0;
}

void ColorReducer::rgbToLab( int ri, int gi, int bi, double& L, double& a, double& b )
{
	// convert rgb range to 0.0 - 1.0
	int cd = m_QuantMethod==QUANT_OCTREE ? m_Root.ColMin : 0;
	int cr = m_QuantMethod==QUANT_OCTREE ? m_Root.ColRange : 255;
	double R = double(ri-cd)/cr;
	double G = double(gi-cd)/cr;
	double B = double(bi-cd)/cr;
	// first convert to XYZ
	R = R>0.04045 ? pow((R+0.055)/1.055,2.4) : R/12.92;
	G = G>0.04045 ? pow((G+0.055)/1.055,2.4) : G/12.92;
	B = B>0.04045 ? pow((B+0.055)/1.055,2.4) : B/12.92;
	double x = (R*41.24 + G*35.75 + B*18.05) / 95.047;
	double y = (R*21.26 + G*71.52 + B* 7.22) / 100.0;
	double z = (R* 1.93 + G*11.92 + B*95.05) / 108.883;
	// now convert to Lab
	x = x > 0.008856 ? pow(x, 1.0/3.0) : 7.787*x+16.0/116.0;
	y = y > 0.008856 ? pow(y, 1.0/3.0) : 7.787*y+16.0/116.0;
	z = z > 0.008856 ? pow(z, 1.0/3.0) : 7.787*z+16.0/116.0;
	L = 116.0*y-16.0;
	a = 500.0*(x-y);
	b = 200.0*(y-z);
}

void ColorReducer::countColors()
{
	// fill the color count map
	m_ColorCounts.clear();
	int n = (1<<m_Depth)-1;
	for( int y = 0; y < m_Height; y++ ) {
		const unsigned char *src = m_pSource + m_SrcStride*y;
		for( int x = 0; x < m_Width; x++ ) {
			// reduce to target depth
			int r = round(round(double(src[2]*n)/255)*255/n);
			int g = round(round(double(src[1]*n)/255)*255/n);
			int b = round(round(double(src[0]*n)/255)*255/n);
			int c = (r << 16) | (g << 8)  | b;
			m_ColorCounts[c]++;
			src += m_SrcPixSize;
		}
	}
}

bool ColorReducer::quantizeColors()
{
	// must have source
	if( !m_pSource ) return false;
	// must be palette target
	if( m_NumCols >= 256 ) return false;

	// init palette
	m_Palette.clear();

	switch( m_QuantMethod )
	{
		case QUANT_POPULARITY:
		{
			std::cout << "POP\n";
			countColors();
			// pick the most occurring
			auto it = m_ColorCounts.begin();
			while( it != m_ColorCounts.end() ) {
				auto pit = m_Palette.begin();
				while( pit != m_Palette.end() ) {
					if( it->second > m_ColorCounts[*pit] ) {
						// higher color
						m_Palette.insert( pit, it->first );
						// remove end if needed
						if( m_Palette.size() > m_NumCols )
							m_Palette.pop_back();
						break;
					}
					++pit;
				}
				// only end on small list
				if( m_Palette.size() < m_NumCols )
					m_Palette.push_back( it->first );
				++it;
			}
			m_ColorCounts.clear();
			break;
		}
		case QUANT_ELIMINATION:
		{
			std::cout << "ELIM\n";
			countColors();
			// sort by pixel count
			std::multimap<int,int> count_map;
			auto it = m_ColorCounts.begin();
			while( it != m_ColorCounts.end() ) {
				count_map.insert( std::pair<int,int>(it->second, it->first) );
				++it;
			}
			m_ColorCounts.clear();
			// remove colors from the top until the number is low enough
			int n = (1<<m_Depth)-1;
			while( count_map.size() > m_NumCols ) {
				int v = count_map.begin()->second;
				int r = v>>16, g = (v&0xFF00)>>8,  b = v&255;
				// find color with smallest error
				int minerr = 0x7FFFFFFF;
				auto it = count_map.begin(), minit = it;
				while( ++it != count_map.end() ) {
					int v2 = it->second;
					int r2 = v2>>16, g2 = (v2&0xFF00)>>8,  b2 = v2&255;
					int err = colorError(r, g, b, r2, g2, b2);
					if( err < minerr ) {
						minit = it;
						minerr = err;
					}
				};
				// add new combined row
				int n = count_map.begin()->first + minit->first;
				v = minit->second;
				count_map.insert( minit, std::pair<int,int>(n, v) );
				// erase old rows
				count_map.erase(minit);
				count_map.erase(count_map.begin());
			}
			// add remaining colors to palette
			auto cit = count_map.begin();
			while( cit != count_map.end() ) {
				int v = cit->second;
				int r = round(double(v>>16)        *n/255);
				int g = round(double((v&0xFF00)>>8)*n/255);
				int b = round(double(v&255)        *n/255);
				m_Palette.push_back( (r<<16) | (g<<8) | b );
				++cit;
			}
			break;
		}
		case QUANT_ERRORELIMINATION:
		{
			std::cout << "ERRELIM\n";
			countColors();
			// sort by pixel count
			std::multimap<int,int> count_map;
			auto it = m_ColorCounts.begin();
			while( it != m_ColorCounts.end() ) {
				count_map.insert( std::pair<int,int>(it->second, it->first) );
				++it;
			}
			m_ColorCounts.clear();
			// remove colors from the top until the number is low enough
			int n = (1<<m_Depth)-1;
			while( count_map.size() > m_NumCols ) {
				int minerr = 0x7FFFFFFF;
				// loop color one
				auto cit1 = count_map.begin(), minit1 = cit1, minit2 = cit1;
				while( cit1 != count_map.end() ) {
					// get color one
					int v = cit1->second;
					int r1 = v>>16, g1 = (v&0xFF00)>>8, b1 = v&255;
					// loop color two
					auto cit2 = cit1;
					++cit2;
					while( cit2 != count_map.end() ) {
						// calculate difference
						v = cit2->second;
						int r2 = v>>16, g2 = (v&0xFF00)>>8, b2 = v&255;
						int err = colorError(r1, g1, b1,  r2, g2, b2) * min(cit1->first, cit2->first);
						if( err < minerr ) {
							minit1 = cit1;
							minit2 = cit2;
							minerr = err;
						}
						++cit2;
					}
					++cit1;
				}
				// add new combined row
				int n = minit1->first + minit2->first;
				if( minit2->first > minit1->first )
					count_map.insert( minit2, std::pair<int,int>(n, minit2->second) );
				else
					count_map.insert( minit1, std::pair<int,int>(n, minit1->second) );
				// erase old rows
				count_map.erase(minit1);
				count_map.erase(minit2);
				count_map.erase(count_map.begin());
			}
			// add remaining colors to palette
			auto cit = count_map.begin();
			while( cit != count_map.end() ) {
				int v = cit->second;
				int r = round(double(v>>16)        *n/255);
				int g = round(double((v&0xFF00)>>8)*n/255);
				int b = round(double(v&255)        *n/255);
				m_Palette.push_back( (r<<16) | (g<<8) | b );
				++cit;
			}
			break;
		}
		case QUANT_OCTREE:
		{
			std::cout << "OCTREE" << int( m_ColorErrorMethod) << "\n";
			// init tree for current depth
			m_Root.reset(m_Depth);
			// build color tree
			for( int y = 0; y < m_Height; y++ ) {
				const unsigned char *src = m_pSource + m_SrcStride*y;
				for( int x = 0; x < m_Width; x++ ) {
					// convert rgb values to target depth
					m_Root.addPixel( src[2], src[1], src[0] );
					src += m_SrcPixSize;
				}
			}
			// reduce color tree
			while( m_Root.ColorCount > m_NumCols ) {
				//debug();
				ColorNode& low = m_Root.findMinError();
				low.remove();
			}
			// compile palette
			m_Root.createPalette(m_Palette);
			break;
		}
		default:
			assert(false);
	}

	// debug output
	for( unsigned int i = 0; i < m_Palette.size(); i++ ) std::cout << i << ": " << std::hex << m_Palette[i] << std::dec << std::endl;
	return true;
}

bool ColorReducer::generateImage()
{
	// must have all data
	if( !m_pSource || !m_pDest ) return false;
	// quantization must be finished
	if( m_NumCols < 256 && m_Palette.size() == 0 ) return false;
	
	// generate data
	if( m_DstStride <= 0 )
		m_DstStride = m_DstPixSize*m_Width;

	
	unsigned char *srcdat = const_cast<unsigned char*>(m_pSource);
	unsigned char *ordered_dither_mat = 0, cthreshR = 0, cthreshG = 0, cthreshB = 0;
	int matw = 1, math = 1, matf = 1;
	if( m_DitherType >= DITHER_FLOYDSTEINBERG ) {
		// error diffusion dithering changes source, use copy
		srcdat = new unsigned char[m_Height*m_SrcStride];
		memcpy( srcdat, m_pSource, m_Height*m_SrcStride );
	} else if( m_DitherType == DITHER_ORDERED ) {
		// flip for calculation
		bool flip = false;
		if( m_OrderY > m_OrderX ) {
			flip = true;
			std::swap(m_OrderX, m_OrderY);
		}
		// ordered dithering, pre-generate matrix
		matw = 1<<m_OrderX, math = 1<<m_OrderY;
		ordered_dither_mat = new unsigned char[matw*math];

		for( int y = 0; y < math; y++ ) {
			for( int x = 0; x < matw; x++ ) {
				int v = 0, offset = 0, maskx = m_OrderX, masky = m_OrderY;
				int xc = x ^ (y << (m_OrderX-m_OrderY)), yc = y, b = 0;
				while( b < m_OrderX+m_OrderY ) {
					v |= ((yc >> --masky)&1) << b++;
					for( offset += m_OrderX; offset >= m_OrderY; offset -= m_OrderY)
						v |= ((xc >> --maskx)&1) << b++;
				}
				if( flip )
					ordered_dither_mat[x*math+y] = v+1;
				else
					ordered_dither_mat[y*matw+x] = v+1;
            }
        }
        // flip back
        if( flip ) {
			std::swap(m_OrderX, m_OrderY);
			std::swap(matw, math);
		}
		// division constant
		matf = matw*math+1;
		// calculate component threshholds TODO: uneven RGB
		cthreshR = 256 / (1<<m_Depth);
		cthreshG = cthreshB = cthreshR;
    }
	
	for( int y = 0; y < m_Height; y++ ) {
		unsigned char *src = srcdat + m_SrcStride*y;
		unsigned char *dst = m_pDest + m_DstStride*y;
		for( int x = 0; x < m_Width; x++ ) {

			if( m_DitherType >= DITHER_FLOYDSTEINBERG && m_DitherType <= DITHER_ATKINSON ) {
				// convert rgb values to target depth
				*dst = getColor( src[2], src[1], src[0] );

				// -- calc error
				int eR = src[2] - round(palRed(*dst)*255);
				int eG = src[1] - round(palGreen(*dst)*255);
				int eB = src[0] - round(palBlue(*dst)*255);
				// get matrix
				ErrorDiffusionMatrix& mat = ErrorDiffusionMatrices[m_DitherType-DITHER_FLOYDSTEINBERG];
				unsigned char *fsd = src + m_SrcPixSize;
				int ox = 1 - mat.offset, oy = 0, el = 0;
				while( oy < mat.height ) {
					// skip if outside image
					int tx = x+ox-mat.offset;
					if( tx >= 0 && tx < m_Width && y+oy < m_Height ) {
						// apply element
						fsd[2] = cclip( fsd[2] + ((eR*mat.matrix[el])/mat.div) );
						fsd[1] = cclip( fsd[1] + ((eG*mat.matrix[el])/mat.div) );
						fsd[0] = cclip( fsd[0] + ((eB*mat.matrix[el])/mat.div) );
					}
					// increase element
					el++;
					ox++;
					fsd += m_SrcPixSize;
					if( ox == mat.width ) {
						ox = 0;
						oy++;
						fsd += m_SrcStride - mat.width*m_SrcPixSize;
					}
				}
			} else if( m_DitherType == DITHER_ORDERED ) {
				// get matrix value
				unsigned char matval = ordered_dither_mat[ (x-m_OffsetX)%matw + matw*((y-m_OffsetY)%math) ];
				unsigned char r = cclip( src[2] + matval * cthreshR / matf );
				unsigned char g = cclip( src[1] + matval * cthreshG / matf );
				unsigned char b = cclip( src[0] + matval * cthreshB / matf );
				*dst = getColor( r, g, b );
			} else  {
				// undithered
				*dst = getColor( src[2], src[1], src[0] );
			}
			// next pixel
			src += m_SrcPixSize;
			dst += m_DstPixSize;
		}
	}
	// clean up
	if( m_DitherType >= DITHER_FLOYDSTEINBERG ) {
		delete [] srcdat;
	} else if( m_DitherType == DITHER_ORDERED ) {
		delete [] ordered_dither_mat;
	}

	return true;
}

int ColorReducer::getColor( int r, int g, int b )
{
	int c = -1, cerr = 0x7FFFFFFF;
	// convert to 0..255
	int n = (1<<m_Depth)-1;
	for( unsigned int i = 0; i < m_Palette.size(); i++ ) {
		int err = colorError( r, g, b,
							  (m_Palette[i] >> 16         )*255/n, 
							 ((m_Palette[i] & 0xFF00) >> 8)*255/n, 
							  (m_Palette[i] & 255         )*255/n );
		if( err < cerr ) {
			c = i;
			cerr = err;
		}
	}
	return c;
}

int ColorReducer::palSize() const
{
	return m_Palette.size();
}

double ColorReducer::palRed( unsigned int c ) const
{
	if( c >= m_Palette.size() ) c = m_Palette.size()-1;
	
	int n = (1<<m_Depth)-1;
	return double(m_Palette[c] >> 16)/n;
}

double ColorReducer::palGreen( unsigned int c ) const
{
	if( c >= m_Palette.size() ) c = m_Palette.size()-1;
	
	int n = (1<<m_Depth)-1;
	return double((m_Palette[c]&0xFF00) >> 8)/n;
}

double ColorReducer::palBlue( unsigned int c ) const
{
	if( c >= m_Palette.size() ) c = m_Palette.size()-1;
	
	int n = (1<<m_Depth)-1;
	return double(m_Palette[c] & 0xFF)/n;
}

void ColorReducer::debug()
{
	std::cout << "Debug output color reducer\n"
	          << "==========================\n";
	std::cout << "Image: (" << m_Width << ", " << m_Height << "), stride: " << m_SrcStride << std::endl;
	std::cout << "Target: " << m_NumCols << " colors/" << m_Depth << " bit\n";
	std::cout << "Color tree:\n";
	m_Root.debug(" ");
	std::cout << "\nColor count:" << m_Root.ColorCount << std::endl;
}


/*
 * ColorNode class
 * 
 * This class represents a node in the rgb color tree.
 */

ColorReducer::ColorNode::ColorNode( ColorReducer *cr )
	: NR(0), NG(0), NB(0)
{
	m_pRootNode = this;
	m_pOwner = cr;
	m_pParent = 0;
	ColorCount = 0;
}

ColorReducer::ColorNode::~ColorNode()
{
}

void ColorReducer::ColorNode::reset( int depth )
{
	// only root node
	assert( this == m_pRootNode );
	// clear children
	clear();
	Level = depth;
	// determine color correction values
	ColMin = 128 >> Level;
	ColRange = ((0xFF80 >> Level) & 255) - ColMin;
	ColFact = double(ColRange)/255;
}

void ColorReducer::ColorNode::clear()
{
	// clear all children
	for( unsigned int i = 0; i < m_Nodes.size(); i++ )
		m_Nodes[i].clear();
	// empty values
	NumPix = NumLPix = 0;;
	RSum = GSum = BSum = 0;
	Error = 0;
}

void ColorReducer::ColorNode::setNode( ColorNode *root, ColorNode *parent, int r, int g, int b )
{
	clear();
	m_pRootNode = root;
	m_pParent = parent;
	m_pOwner = root->m_pOwner;
	Level = m_pParent->Level-1;
	NR = r; NG = g; NB = b;
}

int ColorReducer::ColorNode::colorError( int R1, int G1, int B1, int R2, int G2, int B2 )
{
	return m_pOwner->colorError( R1, G1, B1, R2, G2, B2 );
}

/*
 * Add a pixel to the octree.
 * 
 * When the level is greater than zero, the node is split into eight
 * children (three bit number), which represents whether the R, G or B
 * bit for this level was set or not.
 * 
 * At zero level, the RGB values are added to the total and leaf pixel
 * count is increased. Also, the error with respect to the centre of
 * containing RGB cube is added to the total.
 * 
 * There is a fundamental error in the use octrees that becomes very
 * noticeable on lower palette depths. This is that the octree divides
 * the color space in 2^depth blocks per component. However, values
 * range between minimum and maximum allowing only 2^depth-1 blocks.
 * To compensate for this, the colorspace is shrunk in the root node,
 * resulting in efficively only half a block at either end of the 
 * spectrum.
 * 
 * r, g, b: color components ranging from 0 to 255.
 */ 
void ColorReducer::ColorNode::addPixel( int r, int g, int b )
{
	if( this == m_pRootNode ) {
		r = ColMin+round(ColFact*r);
		g = ColMin+round(ColFact*g);
		b = ColMin+round(ColFact*b);
	}
	// add pixel count
	NumPix++;
	if( Level >  0 ) {
		// handle child nodes
		int lvl = Level-1, bit = 256>>(m_pRootNode->Level-lvl);
		if( !m_Nodes.size() ) {
			// create child nodes
			m_Nodes.resize(8);
			for( int i = 0; i < 8; i++ ) {
				m_Nodes[i].setNode( m_pRootNode, this, NR + (i&4?bit:0), NG + (i&2?bit:0), NB + (i&1?bit:0) );
			}
		}
		// add to subnode
		int v = (r & bit?4:0) + (g & bit?2:0) + (b & bit?1:0);
		m_Nodes[v].addPixel(r, g, b);
	} else {
		// leaf node
		if( NumLPix == 0 ) m_pRootNode->ColorCount++;
		NumLPix++;
		// add color sum
		RSum += r;
		GSum += g;
		BSum += b;
		// calculate error
		int bit = 128 >> m_pRootNode->Level;
		Error += colorError( r, g, b,  NR|bit, NG|bit, NB|bit );
	}
}

long long ColorReducer::ColorNode::error()
{
	if( m_pParent ) {
		if( m_pParent->NumLPix ) {
			// parent has leaf nodes, use error between averages instead
			return NumLPix * colorError( double(RSum)/NumLPix, double(GSum)/NumLPix, double(BSum)/NumLPix,
			                             double(m_pParent->RSum)/m_pParent->NumLPix, double(m_pParent->GSum)/m_pParent->NumLPix, double(m_pParent->BSum)/m_pParent->NumLPix );
		}
	}
	return Error;
}

/*
 * Find the node with the lowest error
 */
ColorReducer::ColorNode& ColorReducer::ColorNode::findMinError()
{
	if( NumPix - NumLPix > 0 ) {
		// nodes contained in children
		// this must have lower error
		ColorNode *low = 0;
		for( int i = 0; i < 8; i++ )
			if( m_Nodes[i].NumPix > 0 ) {
				if( low ) {
					ColorNode& child = m_Nodes[i].findMinError();
					if( child.error() < low->error() )
						low = &child;
				} else
					low = &m_Nodes[i].findMinError();
			}
		return *low;					
	} else 
		return *this;
}

void ColorReducer::ColorNode::remove()
{
	assert( NumPix > 0 );
	assert( NumLPix == NumPix );
	assert( m_pParent );
	// global colorcount reduces if parent already has nodes
	if( m_pParent->NumLPix != 0 )
		m_pRootNode->ColorCount--;
	// move pixels to parent
	m_pParent->RSum += RSum;
	m_pParent->GSum += GSum;
	m_pParent->BSum += BSum;
	RSum = GSum = BSum = 0;
	// The error with respect to the parent node is the error of this
	// node plus the distance between nodes cube centres
	int bit1 = 128>>(m_pParent->Level - Level), bit2 = bit1 << 1;
	m_pParent->Error += Error + NumLPix * colorError(            NR|bit1,            NG|bit1,            NB|bit1,
	                                                  m_pParent->NR|bit2, m_pParent->NG|bit2, m_pParent->NB|bit2 );
	// move nodes
	m_pParent->NumLPix += NumLPix;
	NumPix = NumLPix = 0;
}

void ColorReducer::ColorNode::createPalette( std::vector<int>& pal )
{
	if( NumLPix > 0 ) {
		// calc RGB color
		int n = (1<<m_pRootNode->Level)-1;
		int avgR = round(n*(double(RSum)/NumLPix - m_pRootNode->ColMin) / m_pRootNode->ColRange);
		int avgG = round(n*(double(GSum)/NumLPix - m_pRootNode->ColMin) / m_pRootNode->ColRange);
		int avgB = round(n*(double(BSum)/NumLPix - m_pRootNode->ColMin) / m_pRootNode->ColRange);
		pal.push_back( (avgR << 16) | (avgG << 8) | avgB );
	}
	
	if( Level > 0 )
		for( unsigned int i = 0; i <	m_Nodes.size(); i++ )
			if( m_Nodes[i].NumPix > 0 )
				m_Nodes[i].createPalette(pal);

}

void ColorReducer::ColorNode::debug( std::string indent )
{
	if( NumPix > 0 ) {
		std::cout << indent << "Node: " << NR << ", " << NG << ", " << NB << std::endl;
		std::cout << indent << "Pixels: " << NumLPix << "/"<< NumPix << std::endl;
		std::cout << indent << "Sums: " << RSum << ", " << GSum << ", " << BSum << std::endl;
		std::cout << indent << "Error: " << Error << std::endl;

		for( unsigned int i = 0; i < m_Nodes.size(); i++ )
			m_Nodes[i].debug( indent + "  " );
	}
}


} // namespace Polka 
