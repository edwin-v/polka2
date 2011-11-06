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
