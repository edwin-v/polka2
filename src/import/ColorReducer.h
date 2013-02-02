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

#ifndef _POLKA_COLORREDUCER_H_
#define _POLKA_COLORREDUCER_H_

#include <string>
#include <vector>
#include <map>
#include <glibmm/ustring.h>

namespace Polka {

class ColorReducer
{
public:
	ColorReducer();
	~ColorReducer();

	// techniques applied for color reduction
	enum QuantizationMethod { QUANT_POPULARITY, QUANT_ELIMINATION, 
	                          QUANT_ERRORELIMINATION, QUANT_OCTREE };
	enum ColorErrorMethod { COLORERROR_RGB, COLORERROR_PERCEPTUAL };
	enum DitherType { DITHER_NONE = 0, DITHER_ORDERED,
	                  /* error diffusion dithers */
	                  DITHER_FLOYDSTEINBERG, DITHER_JARVISJUDICENINKE,
	                  DITHER_STUCKI, DITHER_BURKES,DITHER_SIERRA3,
	                  DITHER_SIERRA2, DITHER_SIERRA2_4A,
	                  DITHER_STEVENSONARCE, DITHER_ATKINSON };

	// retrieve information about available targets
	static void getTargetPalettes( std::vector<std::string>& pal_vec );
	static const std::string& getTargetCanvas( const std::string& pal );
	static bool needQuantization( const std::string& pal );
	
	void setRGBSource( const unsigned char *src, int width, int height, int pixsize = 4, int stride = -1 );
	void setTarget( const std::string& palette, unsigned char *dest, int pixsize = 1, int stride = -1 );

	// select technique for color reduction
	void setQuantizationMethod( QuantizationMethod method, ColorErrorMethod error );
	void setDitherType( DitherType type, int orderx = 2, int ordery = 2, int offsetx = 0, int offsety = 0 );

	// apply color reduction
	bool quantizeColors();
	bool generateImage();
	
	// retrieve palette colors
	int palSize() const;
	double palRed( unsigned int c ) const;
	double palGreen( unsigned int c ) const;
	double palBlue( unsigned int c ) const;
	int getColor( int r, int g, int b );
	
	void debug();
	
protected:

private:
	const unsigned char *m_pSource;
	unsigned char *m_pDest;
	int m_Width, m_Height;
	int m_SrcPixSize, m_SrcStride;
	int m_DstPixSize, m_DstStride;
	unsigned int m_NumCols, m_Depth;

	DitherType m_DitherType;
	QuantizationMethod m_QuantMethod;
	ColorErrorMethod m_ColorErrorMethod;
	int m_OrderX, m_OrderY, m_OffsetX, m_OffsetY;

	// color error calculation
	void rgbToLab( int ri, int gi, int bi, double& L, double& a, double& b );
	int colorError( int R1, int G1, int B1, int R2, int G2, int B2 );

	// palette storage after quantization
	std::vector<int> m_Palette;

	// Color list for quantization
	std::map<int,int> m_ColorCounts;

	void countColors();

	// octree quantization node
	class ColorNode
	{
	public:
		ColorNode( ColorReducer *cr = 0 );
		~ColorNode();
	
		void reset( int depth );
		void clear();
		void addPixel( int r, int g, int b );

		long long error();
		ColorNode& findMinError();
		void remove();
		
		void createPalette( std::vector<int>& pal );
		
		void debug( std::string indent );

		// private class, use public member vars for convenience
		// color correction values
		int ColMin, ColRange;
		double ColFact;
		int NumPix, NumLPix;
		int RSum, GSum, BSum;
		int Level, NR, NG, NB;
		long long Error;

		unsigned int ColorCount;

	private:
		std::vector<ColorNode> m_Nodes;
		
		ColorNode *m_pRootNode, *m_pParent;
		ColorReducer  *m_pOwner;

		void setNode( ColorNode *root, ColorNode *parent, int r, int g, int b );
		int colorError( int R1, int G1, int B1, int R2, int G2, int B2 );
	};
	// root node
	ColorNode m_Root;

};


} // namespace Polka

#endif // _POLKA_COLORREDUCER_H_
