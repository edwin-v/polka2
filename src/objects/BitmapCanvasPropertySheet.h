#ifndef _POLKA_BITMAPCANVASPROPERTYSHEET_H_
#define _POLKA_BITMAPCANVASPROPERTYSHEET_H_

#include "ObjectPropertySheet.h"
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/radiobutton.h>

namespace Polka {

class Canvas;

class BitmapCanvasPropertySheet: public ObjectPropertySheet
{
public:
	BitmapCanvasPropertySheet( Canvas& canvas );
	~BitmapCanvasPropertySheet();

	// actions
	virtual void apply() const;
	virtual void reset() const;

private:
	Canvas& m_Canvas;
	Gtk::ComboBoxText m_Templates, m_Palettes;
	Gtk::SpinButton m_Width, m_Height;
	Gtk::RadioButton m_Square, m_High, m_Wide;
	
	void fillTemplates();
	void fillPalettes();
	void useTemplate();
	void initObject();
	void checkChanges();
};

class BitmapCanvasGridSheet: public ObjectPropertySheet
{
public:
	BitmapCanvasGridSheet( Canvas& canvas );
	~BitmapCanvasGridSheet();

	// actions
	virtual void apply() const;
	virtual void reset() const;

private:
	Canvas& m_Canvas;
	Gtk::SpinButton m_HSize, m_VSize, m_HOffset, m_VOffset;

	void initObject();
	void checkChanges();
};

} // namespace Polka

#endif // _POLKA_BITMAPCANVASPROPERTYSHEET_H_
