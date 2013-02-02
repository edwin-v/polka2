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
