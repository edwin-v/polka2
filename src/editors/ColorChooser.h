#ifndef _POLKA_COLORCHOOSER_H_
#define _POLKA_COLORCHOOSER_H_

#include <gtkmm/box.h>
#include "SelectedColorPreview.h"
#include "ColorSelector.h"
#include "Defs.h"

namespace Polka {
	
class Palette;

class ColorChooser : public Gtk::HBox
{
public:
	ColorChooser();
	~ColorChooser();

	// color selection
	void setFGColor( int col );
	void setBGColor( int col );
	int FGColor() const;
	int BGColor() const;

	void setPalette( const Palette *pal );

	IntSignal signalFGChanged();
	IntSignal signalBGChanged();

private:
	SelectedColorPreview m_Preview;
	ColorSelector m_Selector;

	IntSignal m_SignalFGChanged, m_SignalBGChanged;
	
	int m_FGColor, m_BGColor;
	const Palette *m_pPalette;
	
	void onColorClicked( int b );
	void onDoubleClick();

};

} // namespace Polka

#endif // _POLKA_COLORCHOOSER_H_
