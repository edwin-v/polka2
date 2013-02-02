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

#ifndef _POLKA_TOOLBUTTONWINDOW_H_
#define _POLKA_TOOLBUTTONWINDOW_H_

#include <EditorWindow.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/expander.h>
#include <gtkmm/notebook.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/handlebox.h>


namespace Polka {
	
class ToolButtonWindow : public Gtk::HandleBox //public EditorWindow
{
public:
	ToolButtonWindow( int width = 2 );
	~ToolButtonWindow();
	
	void addTool( Glib::RefPtr<Gdk::Pixbuf> icon, Gtk::Widget *w = 0 );
	void activateTool( int id );
	
	// signals
	typedef sigc::signal<void, int> SignalSelectTool;
	SignalSelectTool signalSelectTool();

private:
	int m_Width;
	int m_CurrentTool;
	std::vector<Gtk::ToggleButton*> m_Buttons;
	std::vector<Gtk::Widget*> m_PageWidgets;
	
	Gtk::VBox m_MainBox;
	Gtk::Table m_ButtonGrid;
	Gtk::Expander m_Expand;
	Gtk::Notebook m_Pages;
	Gtk::VBox m_Empty;

	void toolButtonPressed( int id );

	// signal objects
	SignalSelectTool m_SignalSelectTool;

};


} // namespace Polka

#endif // _POLKA_TOOLBUTTONWINDOW_H_
