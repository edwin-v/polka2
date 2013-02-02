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

#ifndef _POLKA_MAINWINDOW_H_
#define _POLKA_MAINWINDOW_H_

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/paned.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/stock.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/frame.h>
#include <gtkmm/recentaction.h>
#include <glibmm/refptr.h>

#include <vector>

#include "EditorWindow.h"
#include "ObjectManager.h"
#include "Project.h"
#include "HistoryWindow.h"


namespace Polka {

class Editor;

class MainWindow : public Gtk::Window 
{
public:
	MainWindow();
	~MainWindow();

protected:
	bool on_delete_event( GdkEventAny *event = 0 );
	void on_hide();

private:
	// action handlers
	void onFileNew();
	void onFileOpen();
	void onFileRecent();
	void onFileSave();
	void onFileSaveAs();
	void onFileImport();
	void onFileClose();
	void onFileQuit();
	void onEditCut();
	void onEditCopy();
	void onEditPaste();
	void onEditDelete();
	void onEditRename();
	void onEditProperties();
	void onEditPreferences();
	void onViewHistory();
	void onHelpAbout();
	
	void onTreeActivate(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
	void activateEditor( Editor *edt );
	void treeUpdated();
	
	void buildActions();

	void addRecentFile( const std::string& file );
	void changeModifiedStatus( UndoHistory::ChangeType type );

	// layout main window
	Gtk::VBox m_MainBox, m_EditBox;
	Gtk::HPaned m_LayoutPane;
	Gtk::ScrolledWindow m_TreeFrame;
	Gtk::Label m_EditorTitle;
	Gtk::Frame m_MainFrame;
	Glib::RefPtr<Gtk::UIManager> m_refUIManager;
	Glib::RefPtr<Gtk::RecentAction> m_refRecent;
	Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
	
	Project *m_pProject;
	Editor* m_MainEditor;
	std::vector<EditorWindow*> m_ActiveEditors;
	
	HistoryWindow m_HistoryWindow;

	Gtk::UIManager::ui_merge_id m_EditorMenuId;
	
	int m_ModifiedCounter;
};

} // namespace Polka 

#endif // _POLKA_MAINWINDOW_H_
