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

#ifndef _POLKA_EDITOR_H_
#define _POLKA_EDITOR_H_

#include <gtkmm/grid.h>
#include <gtkmm/label.h>

#include "EditorWindow.h"

namespace Polka {

class Object;


class Editor : public Gtk::Grid
{
public:
	Editor( const std::string& _id );
	virtual ~Editor();

	void setObject( Polka::Object *obj );
	void treeUpdated();
	virtual void objectUpdated( bool full = true );
	virtual const Glib::ustring& menuString();	
	
	bool isMainEditor() const;
	void activate();
	virtual void hide();

	Glib::ustring title() const;

	virtual void updateDependencies();

	// signals
	//typedef sigc::signal<void, const Glib::ustring> SignalTitleChanged;
	//SignalTitleChanged signalTitleChanged();

protected:
	void setMainEditor( bool value );
	virtual void assignObject( Polka::Object *obj ) = 0;
	virtual void updateTreeNames();
	
private:
	std::string m_Id;
	bool m_MainEditor;
	EditorWindow m_Window;
	Polka::Object *m_pObject;
	
	//SignalTitleChanged m_SignalTitleChanged;
	
	void updateTitle();
};

} // namespace Polka

#endif // _POLKA_EDITOR_H_
