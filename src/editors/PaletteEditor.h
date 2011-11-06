#ifndef _POLKA_PALETTEEDITOR_H_
#define _POLKA_PALETTEEDITOR_H_

#include "Editor.h"
#include "ObjectManager.h"
#include "ColorSlider.h"
#include "ColorPreview.h"
#include "ColorSelector.h"
#include "Palette.h"
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>

namespace Polka {

class PaletteEditor: public Editor
{
public:
	PaletteEditor();
	~PaletteEditor();

	void reset();
	
	void selectColor( int c );
	
protected:
	virtual void assignObject( Polka::Object *obj );

private:
	// signal handlers
	void onChanged();
	void onChanging();
	void onSelect( int c );
	void onClick( int b );
	
	void copyColor();
	void swapColor();
	void createGradient();
	
	ColorSlider m_RedSlider;
	ColorSlider m_GreenSlider;
	ColorSlider m_BlueSlider;
	Gtk::SpinButton m_RedSpin;
	Gtk::SpinButton m_GreenSpin;
	Gtk::SpinButton m_BlueSpin;
	ColorPreview m_Preview;
	ColorSelector m_Selector;
	Gtk::Button m_CopyButton;
	Gtk::Button m_SwapButton;
	Gtk::Button m_GradientButton;
	bool m_Updating;
	
	Palette *m_pPalette;
};

class PaletteEditorFactory : public ObjectManager::EditorFactory
{
public:
	PaletteEditorFactory();

	Editor *create() const;
};


} // namespace Polka

#endif // _POLKA_PALETTEEDITOR_H_
