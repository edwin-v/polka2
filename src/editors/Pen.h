#ifndef _POLKA_PEN_H_
#define _POLKA_PEN_H_


namespace Polka {

class Pen
{
public:
	Pen();
	~Pen();
	
	// pen data access
	int width() const;
	int height() const;
	virtual const int *data() const;

	int offsetX() const;
	int offsetY() const;

	// set data
	virtual void setColor( int col );
	virtual void setData( const int *data );

protected:
	int m_Width, m_Height;
	int	m_DX, m_DY;
	
private:
	int m_Color;

};

} // namespace Polka

#endif // _POLKA_PEN_H_
