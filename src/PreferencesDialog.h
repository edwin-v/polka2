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

#ifndef _POLKA_PREFERENCESDIALOG_H_
#define _POLKA_PREFERENCESDIALOG_H_

#include <gtkmm/dialog.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include "AcceleratorPrefs.h"

namespace Polka {


class PreferencesDialog : public Gtk::Dialog
{
public:
	PreferencesDialog();
	~PreferencesDialog();
	
protected:
	void on_response( int id );

private:
	// list store model columns
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		ModelColumns(){ add(m_ColIcon); add(m_ColName); add(m_PageId); }

		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_ColIcon;
		Gtk::TreeModelColumn< Glib::ustring > m_ColName;
		Gtk::TreeModelColumn< int > m_PageId;
	};

	Glib::RefPtr<Gtk::ListStore> m_refListModel;

	Gtk::HBox m_MainBox;
	Gtk::TreeView m_CategoryList;
	Gtk::Notebook m_PrefsPages;
	ModelColumns m_Columns;

	// pref widgets
	AcceleratorPrefs m_AcceleratorPrefs;

	void addPrefsPage( Glib::RefPtr<Gdk::Pixbuf> icon, const Glib::ustring& name, Gtk::Widget& w );
};

} // namespace Polka

#endif // _POLKA_PREFERENCESDIALOG_H_

