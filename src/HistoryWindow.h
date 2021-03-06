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

#ifndef _POLKA_HISTORYWINDOW_H_
#define _POLKA_HISTORYWINDOW_H_

#include <gtkmm/window.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/box.h>
#include <gtkmm/toolbar.h>
//#include <gtkmm/action.h>

#include "UndoHistory.h"

namespace Polka {

class UndoAction;

class HistoryWindow : public Gtk::Window
{
public:
	HistoryWindow();
	~HistoryWindow();
	
	void setUndoHistory( UndoHistory *hist );
	void setActions( const Glib::RefPtr<Gtk::Action>& undoAction, const Glib::RefPtr<Gtk::Action>& redoAction );
	void onUndoClick();
	void onRedoClick();

protected:
	void on_show();
	void on_hide();

private:
	// list store model columns
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		ModelColumns(){ add(m_ColType); add(m_ColIcon); add(m_ColName); add(m_pAction); }

		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_ColType;
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_ColIcon;
		Gtk::TreeModelColumn< Glib::ustring > m_ColName;
		Gtk::TreeModelColumn< const UndoAction* > m_pAction;
	};

	Gtk::TreeView m_ListView;
	Gtk::HBox m_ButtonBox;
	Gtk::ToolButton m_UndoButton;
	Gtk::ToolButton m_RedoButton;
	Gtk::VBox m_MainBox;
	Glib::RefPtr<Gtk::ListStore> m_refListModel;
	ModelColumns m_Columns;

	Glib::RefPtr< Gdk::Pixbuf > m_refUndoIcon, m_refRedoIcon;

	UndoHistory *m_pHistory;
	sigc::connection m_ConnChange, m_ConnSelection;

	Glib::RefPtr<Gtk::Action> m_refUndoAction, m_refRedoAction;
	
	Gtk::TreeModel::iterator m_itLastUndo;
	Gtk::TreeModel::iterator m_itFirstRedo;
	
	void historyChanged( UndoHistory::ChangeType type );
	void onSelect();
	void clearUndoRows();
	void clearRedoRows();
	void addLastUndoRow();
	void undoAction();
	void redoAction();
	void updateButtonSensitivity();

};

} // namespace Polka

#endif // _POLKA_HISTORYWINDOW_H_

