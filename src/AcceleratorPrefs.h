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

#ifndef _POLKA_ACCELERATORPREFS_H_
#define _POLKA_ACCELERATORPREFS_H_

#include <gtkmm/grid.h>
#include <gtkmm/paned.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include "ActionGrabFrame.h"

namespace Polka {


class AcceleratorPrefs : public Gtk::Grid
{
public:
	AcceleratorPrefs();
	~AcceleratorPrefs();
	
protected:

private:
	// simple single column tree
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		ModelColumns(){ add(m_ColName); add(m_Context); add(m_Id); }

		Gtk::TreeModelColumn< Glib::ustring > m_ColName;
		Gtk::TreeModelColumn< std::string > m_Context;
		Gtk::TreeModelColumn< std::string > m_Id;
	};

	// project tree
	ModelColumns m_Cols;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

	Gtk::Paned m_AccPane;
	Gtk::TreeView m_AccTree;
	Gtk::RadioButton m_RadioDisable, m_RadioLink, m_RadioSet;
	Gtk::ComboBoxText m_ComboLinks;
	Gtk::Label m_NameLabel, m_TypeLabel, m_AccelLabel;
	Gtk::TextView m_DescText;
	ActionGrabFrame m_GrabFrame;
	
	bool m_Updating;
	
	void fillTree();
	void reset();
	Glib::ustring keyDisplayName( guint key, Gdk::ModifierType mods );
	void onAccSelected();
	void onRadioChange();
	void onLinkChange();
	
	void setButton( int button, int modifiers );
	void setKey( int key, int modifiers );
};

} // namespace Polka

#endif // _POLKA_ACCELERATORPREFS_H_

