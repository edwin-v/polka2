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

#ifndef _POLKA_OBJECTPROPERTIESDIALOG_H_
#define _POLKA_OBJECTPROPERTIESDIALOG_H_

#include <gtkmm/dialog.h>
#include <gtkmm/notebook.h>
#include <gtkmm/frame.h>
#include <gtkmm/entry.h>
#include <gtkmm/textview.h>
#include <vector>

namespace Polka {

class Object;
class ObjectPropertySheet;

class ObjectPropertiesDialog : public Gtk::Dialog
{
public:
	ObjectPropertiesDialog( Polka::Object& obj_ );
	~ObjectPropertiesDialog();

protected:
	void on_response( int id );

private:
	Polka::Object& m_Object;
	
	// tabs
	Gtk::Notebook m_Tabs;
	// info tab
	Gtk::VBox m_MainBox;
	Gtk::Label m_NameLabel;
	Gtk::Entry m_NameEntry;
	Gtk::Label m_CommentsLabel;
	Gtk::TextView m_CommentsText;
	// properties tab widgets
	std::vector<ObjectPropertySheet*> m_ObjectPropertyTabs;
	// links
	Gtk::VBox m_LinksBox;
	Gtk::Label    m_UsesLabel;
	Gtk::TextView m_UsesText;
	Gtk::Label    m_UsedByLabel;
	Gtk::TextView m_UsedByText;
	
	bool m_InfoChanged, m_PropertiesChanged;
	
	void setInfoChanged();
	void setPropertiesChanged( bool value );
	void updateSensitivity();
	
};

} // namespace Polka 

#endif // _POLKA_OBJECTPROPERTIESDIALOG_H_
