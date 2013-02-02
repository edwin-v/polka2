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

#ifndef _POLKA_EDITORWINDOW_H_
#define _POLKA_EDITORWINDOW_H_

#include <gtkmm/window.h>
#include <gtkmm/box.h>


namespace Polka {

class Editor;

class EditorWindow : public Gtk::Window 
{
public:
	EditorWindow( const std::string& _id );
	~EditorWindow();

	void setEditor( Editor *edt );
	Editor *editor();

protected:
	void on_show();
	void on_hide();
	bool on_delete_event( GdkEventAny *event );

private:
	std::string m_Id;
	// layout main window
	Gtk::VBox m_MainBox;
	Editor *m_Editor;
};

} // namespace Polka 

#endif // _POLKA_EDITORWINDOW_H_

